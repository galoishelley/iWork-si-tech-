<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<%@ page import="java.util.*,java.text.SimpleDateFormat"%>
<%@ include file="/npage/include/public_title_name_ext.jsp" %>
<%request.setCharacterEncoding("GB2312");%>
<%@ page import="java.text.*"%>
<%@ page contentType="text/html;charset=GB2312" %>
<%

/*

* function description: ������������ë����ֲ��

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
	String opName = "������������ë����ֲ��";
%>
<html><head><title><%=opName%></title>
<script language="JavaScript">

/* operation's onChange */
function changeOpr()
{
	var s_operation = document.frmebp01.s_operation.value;
	if(s_operation == 'A'){
		window.location = "fbp02_activity.jsp";
	}
	if(s_operation == 'P'){
		window.location = "fbp02_payfee.jsp";
	}
	if(s_operation == 'S'){
		window.location = "fbp02_conQry.jsp";
	}
}

</script>

<body>
<form method=post name="frmebp01">
	<%@ include file="/npage/include/header_ext.jsp" %>
	<div class="title">
        <div id="title_zi"> ������������ë����ֲ��</div>
  </div>
  <table  width="100%" border="0" cellspacing="0" cellpadding="0">
  	<tr>
			<td class="Blue" width="15%">
				����ҵ��
			</td>
			<td>
				<select name="s_operation" id="s_operation" class="input-style1" onChange="changeOpr()">
					<option value="" selected >--��ѡ�����ҵ��--</option>
					<option value="S">��Ա�˻���ѯ</option>
					<option value="P">��Ա�ɷ�</option>
					<option value="A">���ϸ¼��</option>
        </select>
     	</td>
    </tr> 	
  </table> 	
    <table width="100%" border="0" cellspacing="0" cellpadding="0">
    	<tr>
    		<td colspan="2" id="footer" align="center">
            <input name="back" onClick="parent._exttabref.removeTab('<%=opCode%>')" type="button" class="b_foot" value="�ر�">
         </td>
      </tr>
    </table>
  </table>
 <%@ include file="/npage/include/footer_ext.jsp" %>
</form>
</body>
</html>    
