
function Initial()
	local parameters = {}
	smparams = {}
	smparams.data = parameters
end

function SetParameter(model_id, key, value)
	if key == nil then
		return
	end
	
	value = (value or "")
	
	local params = smparams.data

	params[key] = value
end

function GetParameter(model_id,key)
	if key == nil then
		return 0
	end

	local params = smparams.data
	if params[key] then
		return 0,params[key]
	else
		return 1,""
	end

end
function CheckParameter(model_id, key)
	if key == nil then
		return 0
	end
	
	local params = smparams.data

	if params[key] then
		return 0
	else
		return 1
	end
end

function GetCurDate()
	return os.date("%Y年%m月%d日")
end

function GetCurDateTime()
	return os.date("%Y年%m月%d日%H时%M分%S秒")
end

function GetNextMonth()
	return "2011年04月01日"
end

function GetSmName()
	local params = smparams.data

	local sm_code = params.SM_CODE
	if sm_code== nil then
		return ""
	end
	if sm_code == "zn" then
		return "神州行"
	elseif sm_code == "dn" then
		return "动感地带"
	elseif sm_code == "gn" then
		return "全球通"
	else
		return ""
	end
end

--[[
Initial()
SetParameter(10,"SendTime","1234567890")
ret,val = GetParameter(10,"SendTime")
print(val)
]]--
