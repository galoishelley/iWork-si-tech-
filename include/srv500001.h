struct req_info
{
	char		qtype;
	char		phone[15];
	char		begtime[16];
	char		endtime[16];
	char		system_type[4];
	char		record_type[4];
	char		colinfo[200];
	char		valinfo[112];
	char		rowinfo[1024];
};
#define	REQINFOLEN	sizeof(struct req_info)

struct req_msg
{
	struct req_info		info;
	char				fname[256];
	char				tout[8];
};
#define	REQMSGLEN	sizeof(struct req_msg)

#define		SUCCALL			"0001"
#define		ZEROINFO		"0000"
#define		FILEERR			"1001"
#define		COMMERR			"1002"
#define		DEFERR			"1003"
#define		LOG				v_debug

struct def_format
{
	char				query_type[8];
	char				system_type[8];
	char				record_type[8];
	char				colinfo[200];
	char				valinfo[112];
	char				rowinfo[200];
	struct def_format	*next;
};

struct billinfo
{
	char		record_type[8];		/* 账单文件类型 */
	char		system_code[8];
	char		record_code[8];
	int		base_col_num;		/* 查询列数量 */
	char		base_col_info[256];	/* 查询列信息 */
	char		row_info[1024];		/* 查询条件   */
	struct billinfo *next;
};

struct billqueryreq
{
	char		bill_code[4];		/* 账单代码 */
	char		head_info[1024];	/* 信息显示题目 */
	int		sort_col;		/* 排序列序号 */
	char		col_null[16];		/* 空字段填充字符 */
	char		is_null;		/* 空字段填充标志 */
	char		col_disp_name[512];	/* 列信息显示名称 */
	char		col_disp_type[512];	/* 列信息显示类型 */
        int		col_disp_num;		/* 列信息显示数量 */
	struct billinfo billinfohead;		/* 查询条件 */
};

struct col_val
{
	char		col_type[4];
	int		total_fee;
};

struct detail_info
{
	char			info[512];
	char			comp[64];
	struct detail_info	*next;
};
