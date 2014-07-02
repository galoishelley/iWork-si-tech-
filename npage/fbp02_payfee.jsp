<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<%@ page import="java.util.*,java.text.SimpleDateFormat"%>
<%@ include file="/npage/include/public_title_name_ext.jsp" %>
<%request.setCharacterEncoding("GB2312");%>
<%@ page import="java.text.*"%>
<%@ page contentType="text/html;charset=GB2312" %>
<%

/*

* function description: 吉林撒欢打羽毛球俱乐部活动

* date: 2013/1/8

* copyright: si-tech

* author: miaoyl

*/

%>	
<%
  String work_no = (String)session.getAttribute("workNo");
  String work_name = (String)session.getAttribute("workName");
  String orgCode = (String)session.getAttribute("orgCode");
  String IpAddr = (String)session.getAttribute("ipAddr");
  String opCode = "bp01";
	String opName = "吉林撒欢打羽毛球俱乐部活动";
	String strSql="SELECT to_char(cust_id),cust_name FROM dbadmintonCustMsg WHERE valid='1' order by cust_name "; 
%>
<html><head><title><%=opName%></title>
<script language="JavaScript">

$(document).ready(
function(){
	document.frmebp01.confirm.disabled=true;
});

/*clean page element*/
function resetJsp()
{
 	with(document.frmebp01)
 	{ 
 		s_custname.value = "";
 		s_activitylist.value = "";
 		s_prefee.value = "";
		s_opnote.value = "";
		document.frmebp01.addCondConfirm.disabled=false;
		document.frmebp01.confirm.disabled=true;
 	}
}

/*submit page*/
function doCheck(){

	var s_prefee=document.all.s_prefee.value.trim();
	if(s_prefee=="")
	{
		rdShowMessageDialog("缴费金额必须输入",1);
		return false;
	}
	
	if(!check(frmebp01))
	{
		return false;
	}
	
	if(forString(document.frmebp01.s_activitylist)){
		var s_activitylist=document.all.s_activitylist.value.trim();
		if(forString(document.frmebp01.s_opnote)){
			var s_opnote=document.all.s_opnote.value.trim();
			if(s_opnote=="")
			{
				s_opnote="会员:"+s_activitylist+" 缴费:"+s_prefee;
				document.all.s_opnote.value=s_opnote;
			}
			document.frmebp01.action="fbp02_p.jsp?s_activitylist="+s_activitylist+"&s_prefee="+s_prefee+"&s_opnote="+s_opnote;
			frmebp01.submit();
		}
	}
	document.frmebp01.confirm.disabled=true;
}

/*go back home*/
function doLoadPage(){
	window.location = "fbp01_1.jsp";
}

/*add member list*/
function badmintonAddRow(){
	var custnametmp=document.frmebp01.s_custname.value;
	if(custnametmp=="")
	{
		rdShowMessageDialog("请先选择会员名称！",1);
		return false;
	}
	var custname=document.frmebp01.s_activitylist.value;
	if(custname==""){
		document.frmebp01.s_activitylist.value=document.frmebp01.s_custname.value.trim();
		document.frmebp01.addCondConfirm.disabled=true;
	}
	document.frmebp01.confirm.disabled=false;
}

</script>
<wtc:service name="TlsPubSelCrm" outnum="2" routerKey="region">
		<wtc:param value="<%=strSql%>" />
		</wtc:service>
<wtc:array id="result" scope="end"/>
</head>

<body>
<form method=post name="frmebp01">
	<%@ include file="/npage/include/header_ext.jsp" %>
	<div class="title">
        <div id="title_zi"> 会员缴费<font color="red">  此功能已经正式使用,因为涉及费用,请勿随便操作！谢谢</font></div>
  </div>
  <table  width="100%" border="0" cellspacing="0" cellpadding="0">
  	<tr>
			<td class="Blue" width="15%">
				会员名称
			</td>
			<td>
				<select name="s_custname" id="s_custname" class="input-style1" >
        <option value="">--请选择会员名称--</option>
        <%
        for(int i=0;i<result.length ;i++){
        %>
         <option value="<%=result[i][1]%>"><%=result[i][1]%></option>
				<%
				}
				%>
        </select> 
        <input name="addCondConfirm" type="button" class="b_text" id="addCondConfirm" value="增加" onClick="badmintonAddRow()">	
     	</td>
      <td class="Blue" width="15%">
				缴费会员名称
			</td>
			<td>
				<input type="text" name="s_activitylist" id="s_activitylist" class="textclass" readonly />
     </td>
    </tr>
    <tr>
  		<td class="Blue" width="15%">
				缴费金额
			</td>
			<td colspan="3">
				<input type="text" name="s_prefee" id="s_prefee" class="textclass" maxLength="4" v_type="float"  v_must="1" />&nbsp;<font color="orange">*</font>
     </td>
  	</tr>	
    </table> 
    <table width="100%" border="0" cellspacing="0" cellpadding="0">
			<tr> 
				<td class="Blue" width="15%">
					系统备注
				</td>
				<td>
					<input type="text" name="s_opnote" id="s_opnote" class="textclass" size="80" maxLength="128" readonly />
				</td>
			</tr>
      <tr> 
        <td colspan="2" id="footer" align="center">
        	  &nbsp;
            <input name="loadpage" type="button" class="b_foot" value="返回" onClick="doLoadPage()">
        	  &nbsp;
            <input name="confirm" type="button" class="b_foot" value="确认" onClick="doCheck()">
            &nbsp; 
            <input name="reset" type="button" class="b_foot" value="清除" onClick="resetJsp()">
            &nbsp; 
            <input name="back" onClick="parent._exttabref.removeTab('<%=opCode%>')" type="button" class="b_foot" value="关闭">
         </td>
      </tr>
    </table>
  </table>
 <%@ include file="/npage/include/footer_ext.jsp" %>
</form>
</body>
</html>    
