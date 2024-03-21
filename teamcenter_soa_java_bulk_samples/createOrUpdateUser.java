package com.teamcenter.hello;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.administration.UserManagementService;
import com.teamcenter.services.strong.administration._2015_07.UserManagement.CreateOrUpdateUserInputs;
import com.teamcenter.services.strong.administration._2015_07.UserManagement.CreateOrUpdateUserResponse;
import com.teamcenter.services.strong.core.SessionService;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;

public class CreateOrUpdateUser 
{
	private UserManagementService umService;
	
	private GroupMember grmMember;
	
	public CreateOrUpdateUser()
	{
		umService = UserManagementService.getService(AppXSession.getConnection());
	}
	
	public void createUser2()
	{
		CreateOrUpdateUserInputs input = new CreateOrUpdateUserInputs();
	
		input.userId = "john";
		input.person = "john";
		input.password = "john";
		input.defaultGroup = "Project Administration";
		
		input.userPropertyMap.put("geography", new String[]{ "US" });
		input.userPropertyMap.put("os_username", new String[]{ "hoonhout" });
		input.userPropertyMap.put("volume", new String[]{ "volume" }); // Name of the Volume not the UID.
		
		CreateOrUpdateUserResponse resp = umService.createOrUpdateUser(new CreateOrUpdateUserInputs[]{ input });
		
		ServiceDataError(resp.serviceData);
	}
	
	protected boolean ServiceDataError(final ServiceData data)
	{
		if(data.sizeOfPartialErrors() > 0)
		{
			for(int i = 0; i < data.sizeOfPartialErrors(); i++)
			{
				for(String msg : data.getPartialError(i).getMessages())
					System.out.println(msg);
			}

			return true;
		}

		return false;
	}
}
