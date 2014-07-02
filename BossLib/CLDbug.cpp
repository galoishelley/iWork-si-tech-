#include "llayerStruct.h"

const char CLDbug::_dig_vec[36+1] =  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#ifndef DBUG_OFF
#define DBUG_OFF
#endif

const char* CLDbug::_DBUG_START_CONDITION_ = "d:t";
CODE_STATE  CLDbug::static_code_state= { 0,0,"?func","?file",0,NULL,NULL,0,"?",0};

#ifndef WIN32
const int	CLDbug::EOS;  // End Of String marker 
const int	CLDbug::PRINTBUF;    // Print buffer size 
const int	CLDbug::INDENT;       // Indentation per trace level 
const int	CLDbug::MAXDEPTH;     // Maximum trace depth default 
const int	CLDbug::TRACE_ON;	// Trace enabled 
const int	CLDbug::DEBUG_ON;	// Debug enabled 
const int	CLDbug::FILE_ON;	// File name print enabled 
const int	CLDbug::LINE_ON;	// Line number print enabled 
const int	CLDbug::DEPTH_ON;	// Function nest level print enabled 
const int	CLDbug::PROCESS_ON;	// Process name print enabled 
const int	CLDbug::NUMBER_ON;	// Number each line // output 
const int	CLDbug::PROFILE_ON;// Print out pr//iling code 
const int	CLDbug::PID_ON;	// Identify each line with process id 
const int	CLDbug::SANITY_CHECK_ON;	// Check safemalloc on DBUG_ENTER 
const int	CLDbug::FLUSH_ON_WRITE;	// Flush on every write 
#endif

int CLDbug::init_done = FALSE; /* Set to TRUE when initialization done */
struct state *CLDbug::stack=0;

/*
* The default file for profiling.  Could also add another flag
* (G?) which allowed the user to specify this.
*
* If the automatic variables get allocated on the stack in
* reverse order from their declarations, then define AUTOS_REVERSE.
* This is used by the code that keeps track of stack usage.  For
* forward allocation, the difference in the dbug frame pointers
* represents stack used by the callee function.  For reverse allocation,
* the difference represents stack used by the caller function.
*
*/

const char*	const CLDbug::PROF_FILE	=	"dbugmon.out";
const char*	const CLDbug::PROF_EFMT	=	"E\t%ld\t%s\n";
const char*	const CLDbug::PROF_XFMT	=	"X\t%ld\t%s\n";

FILE *CLDbug::_db_fp_ = (FILE *) 0;	/* Output stream, default stderr */
const char *CLDbug::_db_process_ = (char*) "dbug"; /* Pointer to process name; argv[0] */
FILE *CLDbug::_db_pfp_ = (FILE *)0;	/* Profile stream, 'dbugmon.out' */
int CLDbug::_db_pon_ = FALSE;	/* TRUE if profile currently on */
int CLDbug::_db_on_ = FALSE;		/* TRUE if debugging currently on */
int CLDbug::_no_db_ = FALSE;		/* TRUE if no debugging at all */
/*
*	Externally supplied functions.
*/


int _sanity(const char *file,ub4 line);


/* Return current user time (ms) */
static unsigned long Clock (void);
static int DelayArg(int value);

/*
*	Miscellaneous printf format strings.
*/

#define ERR_MISSING_RETURN "%s: missing DBUG_RETURN or DBUG_VOID_RETURN macro in function \"%s\"\n"
#define ERR_OPEN "%s: can't open debug output stream \"%s\": "
#define ERR_CLOSE "%s: can't close debug file: "
#define ERR_ABORT "%s: debugger aborting because %s\n"
#define ERR_CHOWN "%s: can't change owner/group of \"%s\": "

/*
*	Macros and defines for testing file accessibility under UNIX and MSDOS.
*/

#undef EXISTS
#if !defined(HAVE_ACCESS) || defined(MSDOS)
#define EXISTS(pathname) (FALSE)	/* Assume no existance */
#define Writable(name) (TRUE)
#else
#define EXISTS(pathname)	 (access (pathname, F_OK) == 0)
#define WRITABLE(pathname)	 (access (pathname, W_OK) == 0)
#endif
#ifndef MSDOS
#define ChangeOwner(name)
#endif

/*
*	Translate some calls among different systems.
*/

#if defined(unix) || defined(xenix) || defined(VMS) || defined(__NetBSD__)
# define Delay(A) sleep((ub4) A)
#elif defined(AMIGA)
int Delay ();			/* Pause for given number of ticks */
#else
static int Delay(int ticks);
#endif


CODE_STATE *CLDbug::code_state(void)
{
	return &static_code_state;
}


/*
*  FUNCTION
*
*	_db_push_	push current debugger state and set up new one
*
*  SYNOPSIS
*
*	VOID _db_push_ (control)
*	char *control;
*
*  DESCRIPTION
*
*	Given pointer to a debug control string in "control", pushes
*	the current debug state, parses the control string, and sets
*	up a new debug state.
*
*	The only attribute of the new state inherited from the previous
*	state is the current function nesting level.  This can be
*	overridden by using the "r" flag in the control string.
*
*	The debug control string is a sequence of colon separated fields
*	as follows:
*
*		<field_1>:<field_2>:...:<field_N>
*
*	Each field consists of a mandatory flag character followed by
*	an optional "," and comma separated list of modifiers:
*
*		flag[,modifier,modifier,...,modifier]
*
*	The currently recognized flag characters are:
*
*		d	Enable output from DBUG_<N> macros for
*			for the current state.	May be followed
*			by a list of keywords which selects output
*			only for the DBUG macros with that keyword.
*			A null list of keywords implies output for
*			all macros.
*
*		D	Delay after each debugger output line.
*			The argument is the number of tenths of seconds
*			to delay, subject to machine capabilities.
*			I.E.  -#D,20 is delay two seconds.
*
*		f	Limit debugging and/or tracing, and profiling to the
*			list of named functions.  Note that a null list will
*			disable all functions.	The appropriate "d" or "t"
*			flags must still be given, this flag only limits their
*			actions if they are enabled.
*
*		F	Identify the source file name for each
*			line of debug or trace output.
*
*		i	Identify the process with the pid for each line of
*			debug or trace output.
*
*		g	Enable profiling.  Create a file called 'dbugmon.out'
*			containing information that can be used to profile
*			the program.  May be followed by a list of keywords
*			that select profiling only for the functions in that
*			list.  A null list implies that all functions are
*			considered.
*
*		L	Identify the source file line number for
*			each line of debug or trace output.
*
*		n	Print the current function nesting depth for
*			each line of debug or trace output.
*
*		N	Number each line of dbug output.
*
*		o	Redirect the debugger output stream to the
*			specified file.  The default output is stderr.
*
*		O	As O but the file is really flushed between each
*			write. When neaded the file is closed and reopened
*			between each write.
*
*		p	Limit debugger actions to specified processes.
*			A process must be identified with the
*			DBUG_PROCESS macro and match one in the list
*			for debugger actions to occur.
*
*		P	Print the current process name for each
*			line of debug or trace output.
*
*		r	When pushing a new state, do not inherit
*			the previous state's function nesting level.
*			Useful when the output is to start at the
*			left margin.
*
*		S	Do function _sanity(_file_,_line_) at each
*			debugged function until _sanity() returns
*			something that differs from 0.
*			(Moustly used with safemalloc)
*
*		t	Enable function call/exit trace lines.
*			May be followed by a list (containing only
*			one modifier) giving a numeric maximum
*			trace level, beyond which no output will
*			occur for either debugging or tracing
*			macros.  The default is a compile time
*			option.
*
*	Some examples of debug control strings which might appear
*	on a shell command line (the "-#" is typically used to
*	introduce a control string to an application program) are:
*
*		-#d:t
*		-#d:f,main,subr1:F:L:t,20
*		-#d,input,output,files:n
*
*	For convenience, any leading "-#" is stripped off.
*	此函数不是线程安全的，应放在新建其它线程之前执行。
*/

void CLDbug::_db_push_ (const char *control)
{
	register char *scan;
	register TDbLink *temp;
	CODE_STATE *state;
	char *new_str;

	if (! _db_fp_)
		_db_fp_= stderr;		/* Output stream, default stderr */

	if (*control == '-')
	{
		if (*++control == '#')
			control++;
	}
	if (*control)
		_no_db_=0;			/* We are using dbug after all */

	new_str = StrDup (control);
	PushState ();
	state=code_state();

	scan = static_strtok (new_str, ':');
	for (; scan != NULL; scan = static_strtok ((char *)NULL, ':'))
	{
		switch (*scan++)
		{
		case 'd':
			_db_on_ = TRUE;
			stack -> flags |= DEBUG_ON;
			if (*scan++ == ',') {
				stack -> keywords = ListParse (scan);
			}
			break;
		case 'D':
			stack -> delay = 0;
			if (*scan++ == ',') {
				temp = ListParse (scan);
				stack -> delay = DelayArg (atoi ((const char*)(temp -> ptr)));
				FreeList (temp);
			}
			break;
		case 'f':
			if (*scan++ == ',') {
				stack -> functions = ListParse (scan);
			}
			break;
		case 'F':
			stack -> flags |= FILE_ON;
			break;
		case 'i':
			stack -> flags |= PID_ON;
			break;
		case 'g':
			_db_pon_ = TRUE;
			if (OpenProfile(PROF_FILE))
			{
				stack -> flags |= PROFILE_ON;
				if (*scan++ == ',')
					stack -> p_functions = ListParse (scan);
			}
			break;
		case 'L':
			stack -> flags |= LINE_ON;
			break;
		case 'n':
			stack -> flags |= DEPTH_ON;
			break;
		case 'N':
			stack -> flags |= NUMBER_ON;
			break;
		case 'A':
		case 'O':
			stack -> flags |= FLUSH_ON_WRITE;
		case 'a':
		case 'o':
			if (*scan++ == ',') {
				temp = ListParse (scan);
				DBUGOpenFile((const char*)(temp -> ptr), (int) (scan[-2] == 'A' || scan[-2] == 'a'));
				FreeList (temp);
			} else {
				DBUGOpenFile ("-",0);
			}
			break;
		case 'p':
			if (*scan++ == ',') {
				stack -> processes = ListParse (scan);
			}
			break;
		case 'P':
			stack -> flags |= PROCESS_ON;
			break;
		case 'r':
			stack->sub_level= state->level;
			break;
		case 't':
			stack -> flags |= TRACE_ON;
			if (*scan++ == ',') {
				temp = ListParse (scan);
				stack -> maxdepth = atoi ((const char*)(temp -> ptr));
				FreeList (temp);
			}
			break;
		case 'S':
			stack -> flags |= SANITY_CHECK_ON;
			break;
		}
	}
	CLMalloc::GlobalFree(new_str);
	new_str = NULL;
}


/*
*  FUNCTION
*
*	_db_pop_    pop the debug stack
*
*  DESCRIPTION
*
*	Pops the debug stack, returning the debug state to its
*	condition prior to the most recent _db_push_ invocation.
*	Note that the pop will fail if it would remove the last
*	valid state from the stack.  This prevents user errors
*	in the push/pop sequence from screwing up the debugger.
*	Maybe there should be some kind of warning printed if the
*	user tries to pop too many states.
*	此函数不是线程安全的，应放在除主线程外所有其它线程结束后执行。
*	
*/

void CLDbug::_db_pop_ ()
{
	register struct state *discard;
	discard = stack;
	if (discard != NULL && discard -> next_state != NULL) {
		stack = discard -> next_state;
		_db_fp_ = stack -> out_file;
		_db_pfp_ = stack -> prof_file;
		if (discard -> keywords != NULL) {
			FreeList (discard -> keywords);
		}
		if (discard -> functions != NULL) {
			FreeList (discard -> functions);
		}
		if (discard -> processes != NULL) {
			FreeList (discard -> processes);
		}
		if (discard -> p_functions != NULL) {
			FreeList (discard -> p_functions);
		}
		CloseFile (discard -> out_file);
		if (discard -> prof_file)
			CloseFile (discard -> prof_file);
		CLMalloc::GlobalFree((char *) discard);
		discard = NULL;
		if (!(stack->flags & DEBUG_ON))
			_db_on_=0;
	}
	else if (discard != NULL)
	{
		CLMalloc::GlobalFree((char *) discard);
		discard = NULL;
		DbugStateDestroy();
	}
	else
	{
		_db_on_=0;
	}
}


/*
*  FUNCTION
*
*	_db_enter_    process entry point to user function
*
*  SYNOPSIS
*
*	VOID _db_enter_ (_func_, _file_, _line_,
*			 _sfunc_, _sfile_, _slevel_, _sframep_)
*	char *_func_;		points to current function name
*	char *_file_;		points to current file name
*	int _line_;		called from source line number
*	char **_sfunc_;		save previous _func_
*	char **_sfile_;		save previous _file_
*	int *_slevel_;		save previous nesting level
*
*  DESCRIPTION
*
*	Called at the beginning of each user function to tell
*	the debugger that a new function has been entered.
*	Note that the pointers to the previous user function
*	name and previous user file name are stored on the
*	caller's stack (this is why the ENTER macro must be
*	the first "executable" code in a function, since it
*	allocates these storage locations).  The previous nesting
*	level is also stored on the callers stack for internal
*	self consistency checks.
*
*	Also prints a trace line if tracing is enabled and
*	increments the current function nesting depth.
*
*	Note that this mechanism allows the debugger to know
*	what the current user function is at all times, without
*	maintaining an internal stack for the function names.
*
*/

void CLDbug::_db_enter_ (
	const char *_func_,
	const char *_file_,
	ub4 _line_,
	const char **_sfunc_,
	const char **_sfile_,
	ub4 *_slevel_)
{
	register CODE_STATE *state;
	if (!_no_db_)
	{
		int save_errno=errno;
		/*
		Sasha: the test below is so we could call functions with DBUG_ENTER
		before my_thread_init(). I needed this because I suspected corruption
		of a block allocated by my_thread_init() itself, so I wanted to use
		my_malloc()/my_free() in my_thread_init()/my_thread_end()
		*/
		if (!(state=code_state()))
			return;
		if (!init_done)
			_db_push_ (_DBUG_START_CONDITION_);

		*_sfunc_ = state->func;
		*_sfile_ = state->file;
		state->func =(char*)  _func_;
		state->file = (char*) _file_;		/* BaseName takes time !! */
		*_slevel_ =  ++state->level;
		if (DoProfile ())
		{
			(void) fprintf (_db_pfp_, PROF_EFMT , Clock (), state->func);
			(void) fflush (_db_pfp_);
		}
		if (DoTrace (state))
		{
			if (!state->locked)
			{
			}
			DoPrefix (_line_);
			Indent (state -> level);
			(void) fprintf (_db_fp_, ">%s\n", state->func);
			dbug_flush (state);			/* This does a unlock */
		}
#ifdef SAFEMALLOC
		if (stack -> flags & SANITY_CHECK_ON)
			if (_sanity(_file_,_line_))		/* Check of safemalloc */
				stack -> flags &= ~SANITY_CHECK_ON;
#endif
		errno=save_errno;
	}
}

/*
*  FUNCTION
*
*	_db_return_    process exit from user function
*
*  SYNOPSIS
*
*	VOID _db_return_ (_line_, _sfunc_, _sfile_, _slevel_)
*	int _line_;		current source line number
*	char **_sfunc_;		where previous _func_ is to be retrieved
*	char **_sfile_;		where previous _file_ is to be retrieved
*	int *_slevel_;		where previous level was stashed
*
*  DESCRIPTION
*
*	Called just before user function executes an explicit or implicit
*	return.  Prints a trace line if trace is enabled, decrements
*	the current nesting level, and restores the current function and
*	file names from the defunct function's stack.
*
*/

void CLDbug::_db_return_ (
	ub4 _line_,
	const char **_sfunc_,
	const char **_sfile_,
	ub4 *_slevel_)
{
	CODE_STATE *state;
	if (!_no_db_)
	{
		int save_errno=errno;
		//printf("ok, i want to know errno=[%d]\n", errno);
		if (!(state=code_state()))
			return;			
		if (!init_done)
			_db_push_ ("");
		if (stack->flags & (TRACE_ON | DEBUG_ON | PROFILE_ON))
		{
			if (!state->locked)
			{
			}
			if (state->level != (int) *_slevel_)
				(void) fprintf (_db_fp_, ERR_MISSING_RETURN, _db_process_,
				state->func);
			else
			{
#ifdef SAFEMALLOC
				if (stack -> flags & SANITY_CHECK_ON)
					if (_sanity(*_sfile_,_line_))
						stack->flags &= ~SANITY_CHECK_ON;
#endif
				if (DoProfile ())
					(void) fprintf (_db_pfp_, PROF_XFMT, Clock(), state->func);
				if (DoTrace (state))
				{
					DoPrefix (_line_);
					Indent (state->level);
					(void) fprintf (_db_fp_, "<%s\n", state->func);
				}
			}
			dbug_flush(state);
		}
		state->level = *_slevel_-1;
		state->func = *_sfunc_;
		state->file = *_sfile_;
		errno=save_errno;
	}
	//printf("ok, i want to know state->level=[%d]\n", state->level);
	//printf("ok, i want to know state->func=[%s]\n", state->func);
	//printf("ok, i want to know state->file=[%s]\n", state->file);
	//printf("ok, i want to know errno=[%d]\n", errno);
}


/*
*  FUNCTION
*
*	_db_pargs_    log arguments for subsequent use by _db_doprnt_()
*
*  SYNOPSIS
*
*	VOID _db_pargs_ (_line_, keyword)
*	int _line_;
*	char *keyword;
*
*  DESCRIPTION
*
*	The new universal printing macro DBUG_PRINT, which replaces
*	all forms of the DBUG_N macros, needs two calls to runtime
*	support routines.  The first, this function, remembers arguments
*	that are used by the subsequent call to _db_doprnt_().
*
*/

void CLDbug::_db_pargs_ (
	ub4 _line_,
	const char *keyword)
{
	CODE_STATE *state=code_state();
	/* Sasha: pre-my_thread_init() safety */
	if (!state)
		return;
	state->u_line = _line_;
	state->u_keyword = (char*) keyword;
}


/*
*  FUNCTION
*
*	_db_doprnt_    handle print of debug lines
*
*  SYNOPSIS
*
*	VOID _db_doprnt_ (format, va_alist)
*	char *format;
*	va_dcl;
*
*  DESCRIPTION
*
*	When invoked via one of the DBUG macros, tests the current keyword
*	set by calling _db_pargs_() to see if that macro has been selected
*	for processing via the debugger control string, and if so, handles
*	printing of the arguments via the format string.  The line number
*	of the DBUG macro in the source is found in u_line.
*
*	Note that the format string SHOULD NOT include a terminating
*	newline, this is supplied automatically.
*
*/

void CLDbug::_db_doprnt_ (const char *format,...)
{
	va_list args;
	CODE_STATE *state;
	/* Sasha: pre-my_thread_init() safety */
	if (!(state=code_state()))
		return;
	//printf("----come in _db_doprnt_----\n");
	va_start(args,format);

	if (_db_keyword_ (state->u_keyword)) {
		int save_errno=errno;
		if (!state->locked)
		{
		}
		DoPrefix (state->u_line);
		if (TRACING()) {
			Indent (state->level + 1);
		} else {
			(void) fprintf (_db_fp_, "%s: ", state->func);
		}
		(void) fprintf (_db_fp_, "%s: ", state->u_keyword);
		(void) vfprintf (_db_fp_, format, args);
		(void) fputc('\n',_db_fp_);
		dbug_flush(state);
		errno=save_errno;
	}
	va_end(args);
	//printf("----come in _db_doprnt_ end ----\n");
}


/*
*  FUNCTION
*
*	      _db_dump_    dump a string until '\0' is found
*
*  SYNOPSIS
*
*	      void _db_dump_ (_line_,keyword,memory,length)
*	      int _line_;		current source line number
*	      char *keyword;
*	      char *memory;		Memory to print
*	      int length;		Bytes to print
*
*  DESCRIPTION
*  Dump N characters in a binary array.
*  Is used to examine corrputed memory or arrays.
*/

void CLDbug::_db_dump_(
	ub4 _line_,
	const char *keyword,
	const char *memory,
	ub4 length)
{
	int pos;
	char dbuff[90];
	CODE_STATE *state;
	/* Sasha: pre-my_thread_init() safety */
	if (!(state=code_state()))
		return;

	if (_db_keyword_ ((char*) keyword))
	{
		if (!state->locked)
		{
		}
		DoPrefix (_line_);
		if (TRACING())
		{
			Indent (state->level + 1);
			pos= (state->level-stack->sub_level>0?state->level-stack->sub_level:0)*INDENT;
			if (pos > 80)
			{
				pos = 80;
			}
		}
		else
		{
			fprintf(_db_fp_, "%s: ", state->func);
		}
		sprintf(dbuff,"%s: Memory: %p  Bytes: (%u)\n",
			keyword,(const char*)memory, length);
		(void) fputs(dbuff,_db_fp_);

		DoPrefix (_line_);
		if (TRACING())
		{
			Indent (state->level + 1);
			pos= (state->level-stack->sub_level>0?state->level-stack->sub_level:0)*INDENT;
			if (pos > 80)
			{
				pos = 80;
			}
		}
		else
		{
			fprintf(_db_fp_, "%s: ", state->func);
		}
		pos=0;
		while (length-- > 0)
		{
			ub4 tmp= *((unsigned char*) memory++);
			if ((pos+=3) >= 80)
			{
				fputc('\n',_db_fp_);
				pos=3;
			}
			/*用十六进制输出字符的ASCII值*/
			fputc(_dig_vec[((tmp >> 4) & 15)], _db_fp_);
			fputc(_dig_vec[tmp & 15], _db_fp_);
			fputc(' ',_db_fp_);
		}
		(void) fputc('\n',_db_fp_);
		dbug_flush(state);
	}
}

/*
*  FUNCTION
*
*	ListParse    parse list of modifiers in debug control string
*
*  SYNOPSIS
*
*	static TDbLink *ListParse (ctlp)
*	char *ctlp;
*
*  DESCRIPTION
*
*	Given pointer to a comma separated list of strings in "cltp",
*	parses the list, building a list and returning a pointer to it.
*	The original comma separated list is destroyed in the process of
*	building the linked list, thus it had better be a duplicate
*	if it is important.
*
*	Note that since each link is added at the head of the list,
*	the final list will be in "reverse order", which is not
*	significant for our usage here.
*
*/

TDbLink *CLDbug::ListParse (	char *ctlp)
{
	register char *start;
	register TDbLink *new_malloc;
	register TDbLink *head;

	head = NULL;
	while (*ctlp != EOS) {
		start = ctlp;
		while (*ctlp != EOS && *ctlp != ',') {
			ctlp++;
		}
		if (*ctlp == ',') {
			*ctlp++ = EOS;
		}
		new_malloc = (TDbLink *) CLMalloc::GlobalMalloc (sizeof (TDbLink));
		new_malloc -> ptr = StrDup (start);
		new_malloc -> next_link = head;
		head = new_malloc;
	}
	return (head);
}

/*
*  FUNCTION
*
*	InList	  test a given string for member of a given list
*
*  SYNOPSIS
*
*	static int InList (linkp, cp)
*	TDbLink *linkp;
*	char *cp;
*
*  DESCRIPTION
*
*	Tests the string pointed to by "cp" to determine if it is in
*	the list pointed to by "linkp".  Linkp points to the first
*	link in the list.  If linkp is NULL then the string is treated
*	as if it is in the list (I.E all strings are in the null list).
*	This may seem rather strange at first but leads to the desired
*	operation if no list is given.	The net effect is that all
*	strings will be accepted when there is no list, and when there
*	is a list, only those strings in the list will be accepted.
*
*/

int CLDbug::InList (TDbLink *linkp,const char *cp)
{
	register TDbLink *scan;
	register int result;

	if (linkp == NULL)
	{
		result = TRUE;
	} else {
		result = FALSE;
		for (scan = linkp; scan != NULL; scan = scan -> next_link) {
			if (STREQ ((const char*)(scan -> ptr), cp)) {
				result = TRUE;
				break;
			}
		}
	}
	return (result);
}


/*
*  FUNCTION
*
*	PushState    push current state onto stack and set up new one
*
*  SYNOPSIS
*
*	static VOID PushState ()
*
*  DESCRIPTION
*
*	Pushes the current state on the state stack, and initializes
*	a new state.  The only parameter inherited from the previous
*	state is the function nesting level.  This action can be
*	inhibited if desired, via the "r" flag.
*
*	The state stack is a linked list of states, with the new
*	state added at the head.  This allows the stack to grow
*	to the limits of memory if necessary.
*
*/

void CLDbug::PushState ()
{
	register struct state *new_malloc;

	if (!init_done)
	{
		DbugStateInit();
	}
	(void) code_state();				/* Alloc memory */
	new_malloc = (struct state *) CLMalloc::GlobalMalloc(sizeof (struct state));
	new_malloc -> flags = 0;
	new_malloc -> delay = 0;
	new_malloc -> maxdepth = MAXDEPTH;
	new_malloc -> sub_level=0;
	new_malloc -> out_file = stderr;
	new_malloc -> prof_file = (FILE*) 0;
	new_malloc -> functions = NULL;
	new_malloc -> p_functions = NULL;
	new_malloc -> keywords = NULL;
	new_malloc -> processes = NULL;
	new_malloc -> next_state = stack;
	stack=new_malloc;
}


/*
*  FUNCTION
*
*	DoTrace    check to see if tracing is current enabled
*
*  SYNOPSIS
*
*	static int DoTrace (stack)
*
*  DESCRIPTION
*
*	Checks to see if tracing is enabled based on whether the
*	user has specified tracing, the maximum trace depth has
*	not yet been reached, the current function is selected,
*	and the current process is selected.  Returns TRUE if
*	tracing is enabled, FALSE otherwise.
*
*/

int CLDbug::DoTrace (CODE_STATE *state)
{
	register int trace=FALSE;

	if (TRACING() &&
		state->level <= stack -> maxdepth &&
		InList (stack -> functions, state->func) &&
		InList (stack -> processes, _db_process_))
		trace = TRUE;
	return (trace);
}


/*
*  FUNCTION
*
*	DoProfile    check to see if profiling is current enabled
*
*  SYNOPSIS
*
*	static int DoProfile ()
*
*  DESCRIPTION
*
*	Checks to see if profiling is enabled based on whether the
*	user has specified profiling, the maximum trace depth has
*	not yet been reached, the current function is selected,
*	and the current process is selected.  Returns TRUE if
*	profiling is enabled, FALSE otherwise.
*
*/

int CLDbug::DoProfile ()
{
	register int profile;
	CODE_STATE *state;
	state=code_state();

	profile = FALSE;
	if (PROFILING() &&
		state->level <= stack -> maxdepth &&
		InList (stack -> p_functions, state->func) &&
		InList (stack -> processes, _db_process_))
		profile = TRUE;
	return (profile);
}


/*
*  FUNCTION
*
*	_db_keyword_	test keyword for member of keyword list
*
*  SYNOPSIS
*
*	int _db_keyword_ (keyword)
*	char *keyword;
*
*  DESCRIPTION
*
*	Test a keyword to determine if it is in the currently active
*	keyword list.  As with the function list, a keyword is accepted
*	if the list is null, otherwise it must match one of the list
*	members.  When debugging is not on, no keywords are accepted.
*	After the maximum trace level is exceeded, no keywords are
*	accepted (this behavior subject to change).  Additionally,
*	the current function and process must be accepted based on
*	their respective lists.
*
*	Returns TRUE if keyword accepted, FALSE otherwise.
*
*/

int CLDbug::_db_keyword_ (
	const char *keyword)
{
	register int result;
	CODE_STATE *state;

	if (!init_done)
		_db_push_ ("");
	/* Sasha: pre-my_thread_init() safety */
	if (!(state=code_state()))
		return FALSE;
	result = FALSE;
	if (DEBUGGING() &&
		state->level <= stack -> maxdepth &&
		InList (stack -> functions, state->func) &&
		InList (stack -> keywords, keyword) &&
		InList (stack -> processes, _db_process_))
		result = TRUE;
	return (result);
}

/*
*  FUNCTION
*
*	Indent	  indent a line to the given indentation level
*
*  SYNOPSIS
*
*	static VOID Indent (indent)
*	int indent;
*
*  DESCRIPTION
*
*	Indent a line to the given level.  Note that this is
*	a simple minded but portable implementation.
*	There are better ways.
*
*	Also, the indent must be scaled by the compile time option
*	of character positions per nesting level.
*
*/

void CLDbug::Indent (int indent)
{
	register int count;

	indent= (indent-1-stack->sub_level>0?indent-1-stack->sub_level:0)*INDENT;
	for (count = 0; count < indent ; count++)
	{
		if ((count % INDENT) == 0)
			fputc('|',_db_fp_);
		else
			fputc(' ',_db_fp_);
	}
}


/*
*  FUNCTION
*
*	FreeList    free all memory associated with a linked list
*
*  SYNOPSIS
*
*	static VOID FreeList (linkp)
*	TDbLink *linkp;
*
*  DESCRIPTION
*
*	Given pointer to the head of a linked list, frees all
*	memory held by the list and the members of the list.
*
*/

void CLDbug::FreeList (TDbLink *linkp)
{
	register TDbLink *old;

	while (linkp != NULL) {
		old = linkp;
		linkp = linkp -> next_link;
		if (old -> ptr != NULL) {
			CLMalloc::GlobalFree(old -> ptr);
			old -> ptr = NULL;
		}
		CLMalloc::GlobalFree((char *) old);
		old = NULL;
	}
}


/*
*  FUNCTION
*
*	StrDup	 make a duplicate of a string in new memory
*
*  SYNOPSIS
*
*	static char *StrDup (my_string)
*	char *string;
*
*  DESCRIPTION
*
*	Given pointer to a string, allocates sufficient memory to make
*	a duplicate copy, and copies the string to the newly allocated
*	memory.  Failure to allocated sufficient memory is immediately
*	fatal.
*
*/
char *CLDbug::StrDup (const char *str)
{
	register char *new_malloc;
	new_malloc = (char*)CLMalloc::GlobalMalloc((size_t) strlen (str) + 1);
	(void) strcpy (new_malloc, str);
	return (new_malloc);
}


/*
*  FUNCTION
*
*	DoPrefix    print debugger line prefix prior to indentation
*
*  SYNOPSIS
*
*	static VOID DoPrefix (_line_)
*	int _line_;
*
*  DESCRIPTION
*
*	Print prefix common to all debugger output lines, prior to
*	doing indentation if necessary.  Print such information as
*	current process name, current source file name and line number,
*	and current function nesting depth.
*
*/
void CLDbug::DoPrefix (ub4 _line_)
{
	CODE_STATE *state;
	state=code_state();

	state->lineno++;
	if (stack -> flags & PID_ON)
	{
#ifdef WIN32
		(void) fprintf (_db_fp_, "%5d: ", (int) _getpid ());
#else
		(void) fprintf (_db_fp_, "%5d: ", (int) getpid ());
#endif
	}
	if (stack -> flags & NUMBER_ON) {
		(void) fprintf (_db_fp_, "%5d: ", state->lineno);
	}
	if (stack -> flags & PROCESS_ON) {
		(void) fprintf (_db_fp_, "%s: ", _db_process_);
	}
	if (stack -> flags & FILE_ON) {
		(void) fprintf (_db_fp_, "%14s: ", BaseName(state->file));
	}
	if (stack -> flags & LINE_ON) {
		(void) fprintf (_db_fp_, "%5d: ", _line_);
	}
	if (stack -> flags & DEPTH_ON) {
		(void) fprintf (_db_fp_, "%4d: ", state->level);
	}
}


/*
*  FUNCTION
*
*	DBUGOpenFile	open new output stream for debugger output
*
*  SYNOPSIS
*
*	static VOID DBUGOpenFile (name)
*	char *name;
*
*  DESCRIPTION
*
*	Given name of a new file (or "-" for stdout) opens the file
*	and sets the output stream to the new file.
*
*/
void CLDbug::DBUGOpenFile (const char *name,int append)
{
	register FILE *fp;
	register int newfile;

	if (name != NULL)
	{
		strcpy(stack->name,name);
		if (strcmp (name, "-") == 0)
		{
			_db_fp_ = stdout;
			stack -> out_file = _db_fp_;
			stack -> flags |= FLUSH_ON_WRITE;
		}
		else
		{
			if (!Writable((char*)name))
			{
				(void) fprintf (stderr, ERR_OPEN, _db_process_, name);
				perror ("");
				fflush(stderr);
			}
			else
			{
				newfile= !EXISTS (name);
				if (!(fp = fopen(name, append ? "a+" : "w")))
				{
					(void) fprintf (stderr, ERR_OPEN, _db_process_, name);
					perror ("");
					fflush(stderr);
				}
				else
				{
					_db_fp_ = fp;
					stack -> out_file = fp;
					if (newfile) {
						ChangeOwner (name);
					}
				}
			}
		}
	}
}


/*
*  FUNCTION
*
*	OpenProfile    open new output stream for profiler output
*
*  SYNOPSIS
*
*	static FILE *OpenProfile (name)
*	char *name;
*
*  DESCRIPTION
*
*	Given name of a new file, opens the file
*	and sets the profiler output stream to the new file.
*
*	It is currently unclear whether the prefered behavior is
*	to truncate any existing file, or simply append to it.
*	The latter behavior would be desirable for collecting
*	accumulated runtime history over a number of separate
*	runs.  It might take some changes to the analyzer program
*	though, and the notes that Binayak sent with the profiling
*	diffs indicated that append was the normal mode, but this
*	does not appear to agree with the actual code. I haven't
*	investigated at this time [fnf; 24-Jul-87].
*/
FILE *CLDbug::OpenProfile (const char *name)
{
	register FILE *fp;
	register int newfile;

	fp=0;
	if (!Writable (name))
	{
		(void) fprintf (_db_fp_, ERR_OPEN, _db_process_, name);
		perror ("");
		dbug_flush(0);
		(void) Delay (stack -> delay);
	}
	else
	{
		newfile= !EXISTS (name);
		if (!(fp = fopen (name, "w")))
		{
			(void) fprintf (_db_fp_, ERR_OPEN, _db_process_, name);
			perror ("");
			dbug_flush(0);
		}
		else
		{
			_db_pfp_ = fp;
			stack -> prof_file = fp;
			if (newfile)
			{
				ChangeOwner (name);
			}
		}
	}
	return fp;
}

/*
*  FUNCTION
*
*	CloseFile    close the debug output stream
*
*  SYNOPSIS
*
*	static VOID CloseFile (fp)
*	FILE *fp;
*
*  DESCRIPTION
*
*	Closes the debug output stream unless it is standard output
*	or standard error.
*
*/
void CLDbug::CloseFile (FILE *fp)
{
	if (fp != stderr && fp != stdout) {
		if (fclose (fp) == EOF)
		{
			(void) fprintf (_db_fp_, ERR_CLOSE, _db_process_);
			perror ("");
			dbug_flush(0);
		}
	}
}

/*
*  FUNCTION
*
*	DbugExit    print error message and exit
*
*  SYNOPSIS
*
*	static VOID DbugExit (why)
*	char *why;
*
*  DESCRIPTION
*
*	Prints error message using current process name, the reason for
*	aborting (typically out of memory), and exits with status 1.
*	This should probably be changed to use a status code
*	defined in the user's debugger include file.
*
*/
void CLDbug::DbugExit (const char *why)
{
	(void) fprintf (stderr, ERR_ABORT, _db_process_, why);
	(void) fflush (stderr);
	exit (1);
}

/**
 *		像strtok函数，但是在一行中的两个分隔将被替代为一个分隔符。程序不支持
 * 线程级的访问。.
 */
char *CLDbug::static_strtok (char *s1, char separator)
{
	static char *end = NULL;
	register char *rtnval,*cpy;

	rtnval = NULL;
	if (s1 != NULL)
		end = s1;
	if (end != NULL && *end != EOS)
	{
		rtnval=cpy=end;
		do
		{
			if ((*cpy++ = *end++) == separator)
			{
				if (*end != separator)
				{
					cpy--;		/* Point at separator */
					break;
				}
				end++;			/* Two separators in a row, skipp one */
			}
		} while (*end != EOS);
		*cpy=EOS;			/* Replace last separator */
	}
	return (rtnval);
}


/*
*  FUNCTION
*
*	BaseName    strip leading pathname components from name
*
*  SYNOPSIS
*
*	static char *BaseName (pathname)
*	char *pathname;
*
*  DESCRIPTION
*
*	Given pointer to a complete pathname, locates the base file
*	name at the end of the pathname and returns a pointer to
*	it.
*
*/
char *CLDbug::BaseName (const char *pathname)
{
	const char *base;

	base = (const char *)strrchr (pathname, FN_LIBCHAR);
	if (base++ == NULL)
		base = pathname;
	return ((char*) base);
}


/*
*  FUNCTION
*
*	Writable    test to see if a pathname is writable/creatable
*
*  SYNOPSIS
*
*	static int Writable (pathname)
*	char *pathname;
*
*  DESCRIPTION
*
*	Because the debugger might be linked in with a program that
*	runs with the set-uid-bit (suid) set, we have to be careful
*	about opening a user named file for debug output.  This consists
*	of checking the file for write access with the real user id,
*	or checking the directory where the file will be created.
*
*	Returns TRUE if the user would normally be allowed write or
*	create access to the named file.  Returns FALSE otherwise.
*
*/


#ifndef Writable

static int Writable (
	char *pathname)
{
	register int granted;
	register char *lastslash;

	granted = FALSE;
	if (EXISTS (pathname)) {
		if (WRITABLE (pathname)) {
			granted = TRUE;
		}
	} else {
		lastslash = strrchr (pathname, '/');
		if (lastslash != NULL) {
			*lastslash = EOS;
		} else {
			pathname = ".";
		}
		if (WRITABLE (pathname)) {
			granted = TRUE;
		}
		if (lastslash != NULL) {
			*lastslash = '/';
		}
	}
	return (granted);
}
#endif


/*
*  FUNCTION
*
*	ChangeOwner    change owner to real user for suid programs
*
*  SYNOPSIS
*
*	static VOID ChangeOwner (pathname)
*
*  DESCRIPTION
*
*	For unix systems, change the owner of the newly created debug
*	file to the real owner.  This is strictly for the benefit of
*	programs that are running with the set-user-id bit set.
*
*	Note that at this point, the fact that pathname represents
*	a newly created file has already been established.  If the
*	program that the debugger is linked to is not running with
*	the suid bit set, then this operation is redundant (but
*	harmless).
*
*/

#ifndef ChangeOwner
static void ChangeOwner (
	char *pathname)
{
	if (chown (pathname, getuid (), getgid ()) == -1)
	{
		(void) fprintf (stderr, ERR_CHOWN, _db_process_, pathname);
		perror ("");
		(void) fflush (stderr);
	}
}
#endif


/*
*  FUNCTION
*
*	_db_setjmp_    save debugger environment
*
*  SYNOPSIS
*
*	VOID _db_setjmp_ ()
*
*  DESCRIPTION
*
*	Invoked as part of the user's DBUG_SETJMP macro to save
*	the debugger environment in parallel with saving the user's
*	environment.
*
*/

#ifdef HAVE_LONGJMP

EXPORT void _db_setjmp_ ()
{
	CODE_STATE *state;
	state=code_state();

	state->jmplevel = state->level;
	state->jmpfunc = state->func;
	state->jmpfile = state->file;
}

/*
*  FUNCTION
*
*	_db_longjmp_	restore previously saved debugger environment
*
*  SYNOPSIS
*
*	VOID _db_longjmp_ ()
*
*  DESCRIPTION
*
*	Invoked as part of the user's DBUG_LONGJMP macro to restore
*	the debugger environment in parallel with restoring the user's
*	previously saved environment.
*
*/

EXPORT void _db_longjmp_ ()
{
	CODE_STATE *state;
	state=code_state();

	state->level = state->jmplevel;
	if (state->jmpfunc) {
		state->func = state->jmpfunc;
	}
	if (state->jmpfile) {
		state->file = state->jmpfile;
	}
}
#endif

/*
*  FUNCTION
*
*	DelayArg   convert D flag argument to appropriate value
*
*  SYNOPSIS
*
*	static int DelayArg (value)
*	int value;
*
*  DESCRIPTION
*
*	Converts delay argument, given in tenths of a second, to the
*	appropriate numerical argument used by the system to delay
*	that that many tenths of a second.  For example, on the
*	amiga, there is a system call "Delay()" which takes an
*	argument in ticks (50 per second).  On unix, the sleep
*	command takes seconds.	Thus a value of "10", for one
*	second of delay, gets converted to 50 on the amiga, and 1
*	on unix.  Other systems will need to use a timing loop.
*
*/

#ifdef AMIGA
#define HZ (50)			      /* Probably in some header somewhere */
#endif

static int DelayArg (
	int value)
{
	ub4 delayarg = 0;

#if (unix || xenix)
	delayarg = value / 10;	/* Delay is in seconds for sleep () */
#endif
#ifdef AMIGA
	delayarg = (HZ * value) / 10; /* Delay in ticks for Delay () */
#endif
	return (delayarg);
}


/*
*	A dummy delay stub for systems that do not support delays.
*	With a little work, this can be turned into a timing loop.
*/

#if ! defined(Delay) && ! defined(AMIGA)
static int Delay (
	int ticks)
{
	return ticks;
}
#endif


/*
*  FUNCTION
*
*	perror	  perror simulation for systems that don't have it
*
*  SYNOPSIS
*
*	static VOID perror (s)
*	char *s;
*
*  DESCRIPTION
*
*	Perror produces a message on the standard error stream which
*	provides more information about the library or system error
*	just encountered.  The argument string s is printed, followed
*	by a ':', a blank, and then a message and a newline.
*
*	An undocumented feature of the unix perror is that if the string
*	's' is a null string (NOT a NULL pointer!), then the ':' and
*	blank are not printed.
*
*	This version just complains about an "unknown system error".
*
*/
void CLDbug::perror (const char *s)
{
	if (s && *s != EOS) {
		(void) fprintf (stderr, "%s: ", s);
	}
	(void) fprintf (stderr, "<unknown system error>\n");
}


/* flush dbug-stream, free mutex lock & wait delay */
/* This is because some systems (MSDOS!!) dosn't flush fileheader */
/* and dbug-file isn't readable after a system crash !! */
void CLDbug::dbug_flush(CODE_STATE *state)
{
	if (stack->flags & FLUSH_ON_WRITE)
	{
#if defined(MSDOS) || defined(__WIN__)
		if (_db_fp_ != stdout && _db_fp_ != stderr)
		{
			if (!(freopen(stack->name,"a",_db_fp_)))
			{
				(void) fprintf(stderr, ERR_OPEN, _db_process_, stack->name);
				fflush(stderr);
				_db_fp_ = stdout;
				stack -> out_file = _db_fp_;
				stack -> flags|=FLUSH_ON_WRITE;
			}
		}
		else
#endif
		{
			(void) fflush (_db_fp_);
			if (stack->delay)
				(void) Delay (stack->delay);
		}
	}
	if (!state || !state->locked)
	{
	}
} /* dbug_flush */


void CLDbug::_db_lock_file()
{
	CODE_STATE *state;
	state=code_state();
	state->locked=1;
}

void CLDbug::_db_unlock_file()
{
	CODE_STATE *state;
	state=code_state();
	state->locked=0;
}

/*
 * Here we need the definitions of the clock routine.  Add your
 * own for whatever system that you have.
 */
static unsigned long Clock (void)
{
	//return clock()*(1000/_CLOCKS_PER_SEC);
	return 0;
}
