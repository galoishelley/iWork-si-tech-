<%@ page contentType="text/html;charset=GBK" %>
<%@ page import="java.io.*" %>
<%@ page import="java.util.*,java.text.SimpleDateFormat"%>
<%@ include file="/npage/include/public_title_name.jsp"%>
<%
	String s_month	= request.getParameter("s_month");
	String e_month		= request.getParameter("e_month");
	String s_custname		= request.getParameter("s_custname");
	String s_conflag 	= request.getParameter("s_conflag");
	String strSqlTotal="SELECT to_char(SUM(add_money)) FROM DBADMINTONPAYFEE WHERE pay_flag='1' AND cust_id<>'118'";
	String s_date = "";
	
	String fileNameGB ="";
	if( s_conflag.equals("1") )
	{
		if( s_custname.equals("00") )
		{
			s_custname="all";
		}
		Calendar calendar = Calendar.getInstance();		//此时打印它获取的是系统当前时间
		calendar.add(Calendar.DATE,0);				
		s_date = new SimpleDateFormat("yyyyMMdd").format(calendar.getTime());
	
		fileNameGB = "会员余额报表"+s_date+"@"+s_custname;
	}	
	else if( s_conflag.equals("2") )
		fileNameGB = "会员账户明细报表"+s_month+"-"+e_month+"@"+s_custname;
	
	s_custname		= request.getParameter("s_custname");
	
	String fileName = new String((fileNameGB).getBytes(),"iso_8859-1");
	response.setHeader("Content-disposition","attachment; filename="+fileName+".xls");
	String org_code		= (String)session.getAttribute("orgCode");
	String regionCode 	= org_code.substring(0,2);
	String districtCode	= org_code.substring(2,4);
	int	rlen	= 0;
%>

<wtc:service name="sbp01Qry" retcode="retCode" retmsg="retMsg" outnum="10" >	
	<wtc:param value="<%=s_custname%>"/>
	<wtc:param value="<%=s_month%>"/>
	<wtc:param value="<%=e_month%>"/>
	<wtc:param value="<%=s_conflag%>"/>
</wtc:service>
<wtc:array id="result" start="2" length="8" scope="end" />
<%
	rlen = result.length;
%>
<HTML>
<HEAD>
	<TITLE>会员账户明细报表</TITLE>
	<META http-equiv=Content-Type content="text/html; charset=gb2312" />
</HEAD>
<BODY>
<FORM name="frm" action="" meTHod="post">
<%if( s_conflag.equals("1") ){%>
<TABLE cellspacing="0" border="1">
		<THEAD>
			<TR>
				<th  nowrap class="Blue">序号</th>
      	<th  nowrap class="Blue">会员名称</th>
      	<th  nowrap class="Blue">余额</th>
      	<th  nowrap class="Blue">更新时间</th>
			</TR>
		</THEAD>
		<TBODY>
		<%
		if(rlen>0){
			for(int s=0;s<rlen;s++){
			%>
			<TR>
				<TD><%=(s+1)%></TD>
				<TD><%=result[s][0]%></TD>
				<% if( Float.parseFloat(result[s][1])<0 ) {%>
					<TD><font color="red"><b><%=result[s][1]%></b></font></TD>
				<%}else{%>
					<TD><%=result[s][1]%></TD>
				<%}%>
				<TD><%=result[s][2]%></TD>
			</TR>
			<%
			}
		}
		%>
		<tr>
				<wtc:service name="TlsPubSelCrm" outnum="1" routerKey="region">
					<wtc:param value="<%=strSqlTotal%>" />
					</wtc:service>
				<wtc:array id="result3" scope="end"/>
				<td colspan="4">BADMINTON账户总剩余费用:
				<%rlen=0;
				rlen=result3.length;
				if(rlen>0){%>
					<font color="blue"><b>
				<%=result3[0][0]%> RMB</b></font>
				<%}%>
				</td>
			</tr>	
		</TBODY>
	</TABLE>
<%}else if( s_conflag.equals("2") ){%>
	<TABLE cellspacing="0" border="1">
		<THEAD>
			<TR>
				<th  nowrap class="Blue">序号</th>
				<th  nowrap class="Blue">会员名称</th>
      	<th  nowrap class="Blue">预存</th>
      	<th  nowrap class="Blue">缴费金额</th>
      	<th  nowrap class="Blue">当前余额</th>
      	<th  nowrap class="Blue">缴费状态</th>
      	<th  nowrap class="Blue">备注</th>
      	<th  nowrap class="Blue">操作用户</th>
      	<th  nowrap class="Blue">操作时间</th>
			</TR>
		</THEAD>
		<TBODY>
		<%
		if(rlen>0){
			for(int s=0;s<rlen;s++){
			%>
			<TR>
				<TD><%=(s+1)%></TD>
				<TD><%=result[s][0]%></TD>
				<TD><%=result[s][1]%></TD>
				<TD><%=result[s][2]%></TD>
				<TD><%=result[s][3]%></TD>
				<TD><%=result[s][4]%></TD>
				<TD><%=result[s][5]%></TD>
				<TD><%=result[s][6]%></TD>
				<TD><%=result[s][7]%></TD>
			</TR>
			<%
			}
		}
		%>	
		</TBODY>
	</TABLE>
<%}%>	
</FORM>
</BODY>
</HTML>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       