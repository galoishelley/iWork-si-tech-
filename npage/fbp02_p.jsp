<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<%request.setCharacterEncoding("gbk");%>
<%@ page contentType="text/html;charset=gbk" %>
<%@ include file="/npage/include/public_title_name_ext.jsp" %>

<%
/*

* function description: 吉林撒欢打羽毛球俱乐部活动

* date: 2013/1/8

* copyright: si-tech

* author: miaoyl

*/
%>
<%
		String orgCode = (String)session.getAttribute("orgCode");
		String regionCode = orgCode.substring(0,2);
  	String opCode = "bp01";
		String opName = "吉林撒欢打羽毛球俱乐部活动";
		String s_activitylist = request.getParameter("s_activitylist");
		String s_prefee = request.getParameter("s_prefee");
		String workNo = (String)session.getAttribute("workNo");
		String s_opnote =request.getParameter("s_opnote"); 
		String ipAddr = (String)session.getAttribute("ipAddr");
		try{
%>
    <wtc:service name="sbp01Upd" retcode="retCode" retmsg="retMsg" routerKey="region" outnum="2" >
			<wtc:param value="<%=s_activitylist%>" />
			<wtc:param value="<%=s_prefee%>" />
			<wtc:param value="<%=workNo%>" />
			<wtc:param value="<%=s_opnote%>" />
			<wtc:param value="<%=ipAddr%>" />
   	</wtc:service>
   	<wtc:array id="result" start="0" length="2" scope="end" />
<%
		if (!result[0][0].equals("000000"))
		{
			System.out.println("-------------fbp01_p--result[0][0]"+result[0][0]);
%>
			<script language="JavaScript">
					rdShowMessageDialog("<%=retMsg%><br>错误代码 '<%=retCode%>'。",0);
					window.location = "fbp02_payfee.jsp";
			</script>
<%	
		}
		else
		{
			System.out.println("-----------fbp01_p--result[0][0]"+result[0][0]);
%>		
			<script language="JavaScript">
					rdShowMessageDialog("<%=retMsg%>",2);
					window.location = "fbp02_payfee.jsp";
			</script>
<%			
		}
}		
		catch(Exception e){
%>
		<script language='jscript'>
				rdShowMessageDialog("调用服务出错！",0);
				window.location = "fbp02_payfee.jsp";
		</script>
<%
	}
%>
</Html> 
