/*****************************/
/*********[ DCONMSG ]**********/
/*****************************/
struct wd_dconmsg_area {
	long		contract_no       		;
	long		con_cust_id       		;
	char		contract_passwd   		[ 9];
	char		bank_cust         		[61];
	double		oddment           		;
	char		belong_code       		[ 8];
	double		prepay_fee        		;
	char		prepay_time       		[20];
	char		status            		[ 2];
	char		status_time       		[20];
	char		post_flag         		[ 2];
	double		deposit           		;
	int		min_ym            		;
	double		owe_fee           		;
	long		account_mark      		;
	char		account_limit     		[ 2];
	char		pay_code          		[ 2];
	char		bank_code         		[ 6];
	char		post_bank_code    		[ 6];
	char		account_no        		[31];
	char		account_type      		[ 2];
};
/*****************************/
/*********[ DMARKMSG ]**********/
/*****************************/
struct wd_dmarkmsg_area {
	long		id_no             		;
	char		open_time         		[20];
	long		init_point        		;
	long		current_point     		;
	long		year_point        		;
	long		add_point         		;
	char		op_time           		[20];
	char		begin_time        		[20];
	long		total_used        		;
	long		total_prize       		;
	long		total_punish      		;
	long		lastyear_point    		;
	long		base_point        		;
	char		sm_code           		[ 3];
};
/*****************************/
/*********[ DCUSTSIM ]**********/
/*****************************/
struct wd_dcustsim_area {
	long		id_no             		;
	char		switch_no         		[21];
	char		sim_no            		[21];
};

/*****************************/
/*********[ DCUSTDOC ]**********/
/*****************************/
struct wd_dcustdoc_area {
	long		cust_id           		;
	char		region_code       		[ 3];
	char		district_code     		[ 3];
	char		town_code         		[ 4];
	char		cust_name         		[61];
	char		cust_passwd       		[ 9];
	char		cust_status       		[ 3];
	char		run_time          		[20];
	char		owner_grade       		[ 3];
	char		owner_type        		[ 3];
	char		cust_address      		[61];
	char		id_type           		[ 3];
	char		id_iccid          		[21];
	char		id_address        		[61];
	char		id_validdate      		[20];
	char		contact_person    		[21];
	char		contact_phone     		[21];
	char		contact_address   		[101];
	char		contact_post      		[ 7];
	char		contact_mailaddress		[61];
	char		contact_fax       		[31];
	char		contact_emaill    		[31];
	char		org_code          		[10];
	char		create_time       		[20];
	char		close_time        		[20];
	long		parent_id         		;
	char		create_note       		[61];
	char		true_flag         		[ 2];
	char		group_id          		[11];
	char		info_type         		[ 3];
	char		agreement_type    		[ 2];
};
/*****************************/
/*********[ DLOGINMSG ]**********/
/*****************************/
struct wd_dloginmsg_area {
	char		login_no          		[ 7];
	char		login_name        		[21];
	char		password          		[17];
	char		power_code        		[31];
	int		power_right       		;
	char		login_flag        		[ 2];
	char		op_time           		[20];
	char		rpt_right         		[ 2];
	char		allow_begin       		[20];
	char		allow_end         		[20];
	char		pass_time         		[20];
	char		expire_time       		[20];
	int		try_times         		;
	char		vilid_flag        		[ 2];
	char		maintain_flag     		[ 2];
	char		org_code          		[10];
	char		region_char       		[101];
	char		district_char     		[101];
	char		relogin_flag      		[ 2];
	char		dept_code         		[21];
	char		login_status      		[ 2];
	char		ip_address        		[16];
	char		run_time          		[20];
	char		contract_phone    		[21];
	char		group_id          		[11];
	char		ipbind_flag       		[ 2];
	char		sendpwd_flag      		[ 2];
	char		vilid_time        		[20];
	int		error_times       		;
	char		last_time         		[20];
        long            person_id                       ;
};

/*****************************/
/*********[ SGRANTPREDETCODE ]**********/
/*****************************/
struct wd_sgrantpredetcode_area {
	char		region_code       		[ 3];
	char		detail_code       		[ 5];
	char		detail_name       		[129];
	long		fav_money         		;
	long		month_fav         		;
	int		month_num         		;
	char		send_flag         		[ 2];
	char		begin_date        		[20];
	char		end_date          		[20];
	char		valid_flag        		[ 2];
};
/*****************************/
/*********[ SGRANTCFG ]**********/
/*****************************/
struct wd_sgrantcfg_area {
	char		region_code       		[ 3];
	char		sell_flag         		[ 2];
	char		sell_code         		[ 5];
	char		sell_name         		[257];
	char		grant_flag        		[ 2];
	char		grant_code        		[ 9];
	char		old_sm            		[ 3];
	char		new_sm            		[ 3];
	char		big_flag          		[ 3];
	char		begin_time        		[20];
	char		end_time          		[20];
	long		mark_min          		;
	long		mark_max          		;
	double		begin_money       		;
	double		end_money         		;
	long		mark_used         		;
	char		mode_code         		[ 9];
	char		rep_flag          		[ 2];
	char		login_no          		[ 7];
	char		op_time           		[20];
	char		op_note           		[257];
	char		begin_date        		[20];
	char		end_date          		[20];
	long		mark_add          		;
	long		login_accept      		;
	char		det_mode_code     		[ 5];
};
/*****************************/
/*********[ SGRANTMARKCODE ]**********/
/*****************************/
struct wd_sgrantmarkcode_area {
	char		region_code       		[ 3];
	char		detail_code       		[ 5];
	char		mark_fav_code     		[ 5];
	long		grant_mark        		;
	char		mark_addtype      		[ 2];
	char		mark_minustype    		[ 2];
	char		note              		[61];
};
/*****************************/
/*********[ DCUSTMSG2 ]**********/
/*****************************/
struct wd_dcustmsg_area {
	long		id_no             		;
	long		cust_id           		;
	long		contract_no       		;
	int		ids               		;
	char		phone_no          		[16];
	char		sm_code           		[ 3];
	char		service_type      		[ 3];
	char		attr_code         		[ 9];
	char		user_passwd       		[ 9];
	char		open_time         		[20];
	char		belong_code       		[ 8];
	double		limit_owe         		;
	char		credit_code       		[ 2];
	int		credit_value      		;
	char		run_code          		[ 3];
	char		run_time          		[20];
	char		bill_date         		[20];
	int		bill_type         		;
	char		encrypt_prepay    		[17];
	int		cmd_right         		;
	char		last_bill_type    		[ 2];
	char		bak_field         		[13];
	char		group_id          		[21];
	char		group_no          		[11];
};

/*****************************/
/*********[ SPRESENTCFG ]**********/
/*****************************/
struct wd_spresentcfg_area {
	char		op_code           		[ 5];
	char		region_code       		[ 3];
	char		fav_code          		[21];
	long		begin_money       		;
	long		end_money         		;
	long		fav_money         		;
	long		month_fav         		;
	int		month_num         		;
	char		login_no          		[ 7];
	long		login_accept      		;
	int		least_fee         		;
	char		grant_code        		[ 9];
	char		det_mode_code     		[ 5];
	int		innet_min         		;
	int		innet_max         		;
};

/*****************************/
/*********[ SPAYUSEFLAG ]**********/
/*****************************/
struct wd_spayuseflag_area {
	char		region_code       		[ 3];
	char		grant_code        		[ 9];
	char		pay_type          		[ 2];
	double		prepay_fee        		;
	char		note              		[61];
};
/*****************************/
/*********[ SBILLMODECODE ]**********/
/*****************************/
struct wd_sbillmodecode_area {
	char		region_code       		[ 3];
	char		mode_code         		[ 9];
	char		mode_name         		[31];
	char		sm_code           		[ 3];
	char		mode_flag         		[ 2];
	char		mode_type         		[ 2];
	char		op_code           		[ 5];
	char		note              		[101];
	int		order_fav         		;
	char		start_time        		[20];
	char		stop_time         		[20];
	char		select_flag       		[ 2];
	char		contral_code      		[ 6];
	int		direct_id         		;
        char            before_ctrl_code                [ 6];
        char            end_ctrl_code                   [ 6];
        char            show_flag                       [ 2];
        char            mode_status                     [ 2];
};

/*****************************/
/*********[ SSYSERRMSG ]**********/
/*****************************/
struct wd_ssyserrmsg_area {
	char		errcode           		[ 7];
	char		errmsg            		[257];
};
/*****************************/
/*********[ DPADCECBUSI ]**********/
/*****************************/
struct wd_dpadcecbusi_area {
        long            loginaccept                     ;
        char            ecid                            [15];
        char            oprnumb                         [18];
        char            oprcode                         [ 3];
        char            bizcode                         [19];
        char            biztype                         [ 4];
        char            bizname                         [257];
        char            accessmodel                     [ 3];
        char            accessnumber                    [129];
        long            price                           ;
        char            billingtype                     [ 3];
        char            bizpri                          [ 3];
        char            bizstatus                       [ 2];
        char            rblist                          [ 2];
        char            oprefftime                      [20];
        char            provurl                         [129];
        char            usagedesc                       [513];
        char            introurl                        [129];
        long            lisencenum                      ;
        char            recordtime                      [20];
};
/*****************************/
/*********[ DGRPUSERMSG ]**********/
/*****************************/
struct wd_dgrpusermsg_area {
        long            cust_id                         ;
        long            id_no                           ;
        long            account_id                      ;
        char            user_no                         [16];
        int             ids                             ;
        char            user_name                       [61];
        char            product_type                    [ 5];
        char            product_code                    [ 9];
        char            user_passwd                     [ 9];
        char            login_name                      [21];
        char            login_passwd                    [21];
        char            prov_code                       [ 3];
        char            region_code                     [ 3];
        char            district_code                   [ 3];
        char            town_code                       [ 4];
        char            territory_code                  [21];
        double          limit_owe                       ;
        char            credit_code                     [ 3];
        double          credit_value                    ;
        char            run_code                        [ 2];
        char            old_run                         [ 2];
        char            run_time                        [20];
        char            bill_date                       [20];
        int             bill_type                       ;
        char            last_bill_type                  [ 2];
        char            op_time                         [20];
        char            bak_field                       [17];
        char            sm_code                         [ 3];
        char            group_id                        [11];
};

/*****************************/
/*********[ DPARTERMSG ]**********/
/*****************************/
struct wd_dpartermsg_area {
	char		parter_id         		[21];
	char		parter_name       		[101];
	char		parter_type       		[ 3];
	char		product_info      		[201];
	char		client_info       		[201];
	char		parter_leader     		[101];
	char		link_name         		[101];
	char		link_address      		[201];
	char		link_phone        		[61];
	char		postalcode        		[11];
	char		net_linkman       		[101];
	char		operation_linkman 		[101];
	char		customerservice_link		[101];
	char		customerservice_phon		[61];
	char		ip_number         		[61];
	char		acc_number        		[61];
	char		hq_address        		[201];
	double		money             		;
	char		aptitude_info     		[201];
	char		matter_info       		[201];
	char		terminal_info     		[201];
	char		belong_code       		[ 8];
	char		coop_type         		[ 3];
	char		practice_date     		[20];
	char		businesslicense_numb		[61];
	char		dots_area         		[61];
	char		dots_location     		[101];
	char		tax_number        		[61];
	double		enrol_fund        		;
	char		bank_name         		[101];
	char		account_name      		[101];
	char		note              		[201];
	char		trade_advantage   		[101];
	char		afterservice_type 		[ 3];
	char		id_type           		[ 2];
	char		id_number         		[61];
	char		proposer          		[61];
	char		apply_date        		[20];
	long		worker_number     		;
	char		size_info         		[201];
	char		product_type      		[ 3];
	char		maintain_area     		[101];
	long		checkdoc_id       		;
	char		check_result      		[201];
	char		check_login       		[ 7];
	char		check_date        		[20];
	char		operation_data    		[101];
	char		net_data          		[101];
	char		host_res          		[101];
	char		serveprovide_type 		[ 3];
	char		si_type           		[ 3];
	char		net_res           		[101];
	char		infolicense_number		[61];
	char		banklicense_number		[61];
	char		coop_size         		[ 3];
	char		coopmode_type     		[ 3];
	char		status            		[ 3];
	char		develop           		[201];
	char		grade_id          		[ 3];
	long		cust_id           		;
	char		region_code       		[ 3];
	char		district_code     		[ 3];
	char		town_code         		[ 4];
	char		service_no        		[ 7];
	char		valid_flag        		[ 2];
	char		service_phone     		[41];
	char		district_name     		[101];
	char		acc_name          		[101];
	char		two_check_login   		[ 7];
	char		two_check_date    		[20];
	char		file_one          		[101];
	char		file_four         		[101];
	char		file_three        		[101];
	char		file_two          		[101];
};

/*****************************/
/*********[ SBILLSPCODE ]**********/
/*****************************/
struct wd_sbillspcode_area {
	char		srv_code          		[ 5];
	char		enterprice_code   		[11];
	char		product_code      		[19];
	char		product_note      		[129];
	char		fav_type          		[ 2];
	char		addfee_type       		[ 2];
	int		order_code        		;
	char		bill_status       		[ 2];
	char		net_attr          		[ 5];
	char		price_code        		[ 5];
	char		ip_address        		[16];
	char		protocal          		[13];
	char		trans_port        		[11];
	char		url               		[61];
	int		direct_id         		;
	char		biztype           		[ 4];
	char		bizname           		[257];
	char		accessmodel       		[ 3];
	char		accessnumber      		[129];
	char		billingtype       		[ 3];
	char		bizstatus         		[ 2];
	char		introurl          		[129];
	double		price             		;
        char            valid_date                      [ 9];
        char            istextsign                      [ 2];
        char            defaultsignlang                 [ 2];
        char            textsignen                      [19];
        char            textsignzh                      [19];
        char            isprecharge                     [ 2];
        long            maxitemperday                   ;
        long            maxitempermon                   ;
};

struct wd_sruncode_area {
        char            region_code                     [ 3];
        char            run_code                        [ 2];
        char            run_name                        [13];
        double          month_rate                      ;
        char            normal_flag                     [ 2];
        char            inuse_flag                      [ 2];
        char            remonth_flag                    [ 2];
        int             month_flag                      ;
        int             day_flag                        ;
        double          month_fee                       ;
};
/*****************************/
/*********[ DPADCSIGN ]**********/
/*****************************/
struct wd_dpadcsign_area {
        long            loginaccept                     ;
        long            pkgseq                          ;
        char            mobnum                          [16];
        char            oprnumb                         [18];
        char            oprcode                         [ 3];
        char            efft                            [20];
        char            bizcode                         [33];
        char            ecid                            [15];
        char            recordtime                      [20];
        char            dealflag                        [ 2];
        char            dealtime                        [20];
        char            trytimes                        [15];
};
/*****************************/
/*********[ DIAGWSIGN ]**********/
/*****************************/
struct wd_diagwsign_area {
        long            loginaccept                     ;
        long            pkgseq                          ;
        char            mobnum                          [12];
        char            oprnumb                         [18];
        char            oprcode                         [ 3];
        char            efft                            [20];
        char            bizcode                         [33];
        char            recordtime                      [20];
        char            dealflag                        [ 2];
        char            dealtime                        [20];
        char            domain                          [ 2];
        char            ecid                            [15];
        char            trytimes                        [15];
	char            servcode                        [22];
};
/*****************************/
/*********[ SREGIONCODE ]**********/
/*****************************/
struct wd_sregioncode_area {
	char		region_code       		[ 3];
	char		region_name       		[21];
	char		local_code        		[ 2];
	char		toll_no           		[13];
	char		ip_address        		[21];
	long		min_accept        		;
	char		login_region      		[ 2];
};
/*****************************/
/*********[ SDISCODE ]**********/
/*****************************/
struct wd_sdiscode_area {
	char		district_code     		[ 3];
	char		region_code       		[ 3];
	char		district_name     		[21];
	char		login_district    		[ 2];
	char		area_flag         		[ 2];
};
/*****************************/
/*********[ STOWNCODE ]**********/
/*****************************/
struct wd_stowncode_area {
	char		town_code         		[ 4];
	char		region_code       		[ 3];
	char		district_code     		[ 3];
	char		town_name         		[61];
	char		innet_type        		[ 3];
	char		doc_flag          		[ 2];
	char		fee_type          		[ 2];
	char		login_town        		[ 3];
	char		town_address      		[61];
	char		town_phone        		[41];
	char		contact_person    		[61];
	char		contact_phone     		[31];
	char		contact_id        		[21];
	char		area_code         		[ 3];
	char		area_type         		[ 2];
};

/*****************************/
/*********[ DPERSONMSG ]**********/
/*****************************/
struct wd_dpersonmsg_area {
        long            person_id                       ;
        char            id_type                         [ 3];
        char            person_name                     [21];
        char            id_iccid                        [21];
        char            id_address                      [61];
        char            id_validdate                    [20];
        char            contact_phone                   [21];
        char            contact_call                    [21];
        char            post                            [ 7];
        char            email                           [61];
        char            fax                             [31];
        char            group_id                        [11];
};

/*****************************/
/*********[ SPOWERCODE ]**********/
/*****************************/
struct wd_spowercode_area {
	char		power_code        		[31];
	char		power_name        		[21];
	char		roletype_code     		[ 3];
	char		power_describe    		[256];
	char		create_login      		[ 7];
	char		use_flag          		[ 2];
	char		create_date       		[20];
	char		op_note           		[256];
};

/*****************************/
/*********[ SLOGINPDOMRELATION ]**********/
/*****************************/
struct wd_sloginpdomrelation_area {
	int		popedom_code      		;
	char		login_no          		[ 7];
	char		rela_type         		[ 2];
	char		begin_date        		[20];
	char		end_date          		[20];
	char		grant_login       		[ 7];
	char		op_time           		[20];
};
/*****************************/
/*********[ SPOPEDOMCODE ]**********/
/*****************************/
struct wd_spopedomcode_area {
        int             popedom_code                    ;
        char            pdt_code                        [ 3];
        char            popedom_name                    [31];
        char            use_flag                        [ 2];
        int             order_code                      ;
        char            creat_login                     [ 7];
        char            reflect_code                    [21];
        int             par_domcode                     ;
        char            leaf_flag                       [ 2];
};
