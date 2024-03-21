/* This example was tested with the java HelloTcSoap example provided in the soa_client.zip file.
It assumes you have the HelloTcSoap example working correctly. 

This example will demonstrate how to list Group and Roles for a user.
 
Add the following XML code to the HelloTcSoap Build.xml file (after other codegenwsdlfilename nodes) 
to auto-generate the needed java stub classes.
-----------------------------------------------------------------------------------------------
 
 <codegen wsdlfilename="${soa.home}/wsdls/Core0709DataManagementService.wsdl"
    	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
  <codegen wsdlfilename="${soa.home}/wsdls/Query0603SavedQueryService.wsdl"
      	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
  <codegen wsdlfilename="${soa.home}/wsdls/Query0709SavedQueryService.wsdl"
      	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
 
Add the following code the the Hello.java to call/use the custom java code.
---------------------------------------------------------------------------
 
	GroupMembers gm = new GroupMembers();
  		
  	gm.listUserGroupRoles(user);

	
Add the following class to package com.teamcenter.hellosoap.
------------------------------------------------------------ */

package com.teamcenter.hellosoap;

import java.math.BigInteger;
import java.rmi.RemoteException;


import com.teamcenter.schemas.core._2007_09.datamanagement.LoadObjectsInput;
import com.teamcenter.schemas.core._2008_06.session.SetObjectPropertyPolicyInput;
import com.teamcenter.schemas.query._2006_03.savedquery.GetSavedQueriesInput;
import com.teamcenter.schemas.query._2006_03.savedquery.GetSavedQueriesResponse;
import com.teamcenter.schemas.query._2006_03.savedquery.SavedQueryObject;
import com.teamcenter.schemas.query._2007_09.savedquery.ExecuteSavedQueriesInput;
import com.teamcenter.schemas.query._2007_09.savedquery.QueryInput;
import com.teamcenter.schemas.query._2007_09.savedquery.QueryResults;
import com.teamcenter.schemas.query._2007_09.savedquery.SavedQueriesResponse;
import com.teamcenter.schemas.soa._2006_03.base.ModelObject;
import com.teamcenter.schemas.soa._2006_03.base.ObjectPropertyPolicy_type0;
import com.teamcenter.services.core._2007_09.Core0709DataManagementService;
import com.teamcenter.services.core._2008_06.Core0806SessionService;
import com.teamcenter.services.query._2006_03.InternalServerFaultFault;
import com.teamcenter.services.query._2006_03.InvalidUserFaultFault;
import com.teamcenter.services.query._2006_03.Query0603SavedQueryService;
import com.teamcenter.services.query._2006_03.ServiceFaultFault;
import com.teamcenter.services.query._2007_09.Query0709SavedQueryService;

import com.teamcenter.schemas.soa._2006_03.base.ErrorStack;
import com.teamcenter.schemas.soa._2006_03.base.ErrorValue;
import com.teamcenter.schemas.soa._2006_03.base.Modifiers;
import com.teamcenter.schemas.soa._2006_03.base.PolicyProperty;
import com.teamcenter.schemas.soa._2006_03.base.PolicyType;
import com.teamcenter.schemas.soa._2006_03.base.Property;
import com.teamcenter.schemas.soa._2006_03.base.PropertyValue;
import com.teamcenter.schemas.soa._2006_03.base.ServiceData;


public class GroupMembers
{
	private Core0709DataManagementService dmService0709;
	private Query0603SavedQueryService queryService0603;
	private Query0709SavedQueryService queryService0709;
	private final String savedQuery = "__EINT_group_members"; // OOTB ImanQuery Name.

	private ModelObject query = null;

	public GroupMembers()
	{
		dmService0709 = (Core0709DataManagementService)Session.createServiceStub(Core0709DataManagementService.class);
		queryService0603 = (Query0603SavedQueryService)Session.createServiceStub(Query0603SavedQueryService.class);
		queryService0709 = (Query0709SavedQueryService)Session.createServiceStub(Query0709SavedQueryService.class);

		setObjectPolicy();
	}

	void listUserGroupRoles(final ModelObject user)
	{
		LoadObjectsInput[] loadObjs = getGroupMembersUIDs(user);

		for(LoadObjectsInput input : loadObjs)
		{
			try
			{
				ServiceData sData = dmService0709.loadObjects(input);
			
				if (!ServiceDataError(sData))
				{
					ClientDataModel.addObjects(sData);
					ModelObject[] objs = ClientDataModel.getObjects(sData.getPlainObjs());
					
					for(ModelObject obj : objs)
					{
						if (obj.getClassName().equals("GroupMember"))
						{
							for(Property prop : obj.getProperties())
							{		
								for(PropertyValue value : prop.getValues())
									System.out.println(prop.getName() + " : " +value.getValue());
							}
						}
					}
				}
			}
			catch (RemoteException e)
			{
				e.printStackTrace();
			}
			catch (com.teamcenter.services.core._2007_09.InternalServerFaultFault e)
			{
				e.printStackTrace();
			}
			catch (com.teamcenter.services.core._2007_09.InvalidUserFaultFault e)
			{
				e.printStackTrace();
			}
		}
	}

	protected LoadObjectsInput[] getGroupMembersUIDs(final ModelObject user)
	{
		if (setSaveQuery())
		{
			try
			{
				ExecuteSavedQueriesInput input = new ExecuteSavedQueriesInput();
				QueryInput qryInput = new QueryInput();
				String userName = getPropertyValue(user, "user_name");
				
				qryInput.setMaxNumToReturn(new BigInteger("0"));
				qryInput.setQuery(query);
				qryInput.setResultsType(new BigInteger("0"));
				qryInput.setEntries( new String[] { "User" });
				qryInput.setValues(new String[] { userName });
				
				input.setInput(new QueryInput[]{ qryInput });

				SavedQueriesResponse qryResp = queryService0709.executeSavedQueries(input);
				
				qryResp.getServiceData();

				if (!ServiceDataError(qryResp.getServiceData()))
				{
					int i = 0;
					LoadObjectsInput[] uids = new LoadObjectsInput[qryResp.getArrayOfResults().length];

					for (QueryResults qry : qryResp.getArrayOfResults())
					{
						LoadObjectsInput loadObj = new LoadObjectsInput();
						
						for(String uid : qry.getObjectUIDS())
							loadObj.addUids(uid);
						
						uids[i] = loadObj;
						i++;
					}

					return uids;
				}
			}
			catch (RemoteException e)
			{
				e.printStackTrace();
			}
			catch (com.teamcenter.services.query._2007_09.InternalServerFaultFault e)
			{
				e.printStackTrace();
			}
			catch (com.teamcenter.services.query._2007_09.InvalidUserFaultFault e)
			{
				e.printStackTrace();
			}
		}

		return new LoadObjectsInput[0];
	}

	protected boolean setSaveQuery()
	{
		try
		{
			GetSavedQueriesResponse qryResp = queryService0603.getSavedQueries(new GetSavedQueriesInput());

			if (!ServiceDataError(qryResp.getServiceData()))
			{
				for (SavedQueryObject qObj : qryResp.getQueries())
				{
					if (qObj.getName().equals(savedQuery))
					{
						query = qObj.getQuery();
						return true;
					}
				}
			}
		}
		catch (InternalServerFaultFault e)
		{
			e.printStackTrace();
		}
		catch (ServiceFaultFault e)
		{
			e.printStackTrace();
		}
		catch (InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}
		catch (RemoteException e)
		{
			e.printStackTrace();
		}

		return false;
	}
	
	protected String getPropertyValue(final ModelObject obj, final String propName)
	{
		for(Property prop : obj.getProperties())
		{					
			if(prop.getName().equals(propName))
				for(PropertyValue value : prop.getValues())
					return value.getValue();
		}
		
		return null;
	}
	
	protected boolean ServiceDataError(final ServiceData data)
	{
		if (data.isPartialErrorsSpecified())
		{
			for(ErrorStack error : data.getPartialErrors())
			{
				for (ErrorValue value : error.getErrorValues())
					System.out.println(value.getMessage());
			}

			return true;
		}

		return false;
	}

	protected void setObjectPolicy()
	{
		Core0806SessionService session = (Core0806SessionService)Session.createServiceStub(Core0806SessionService.class);
		SetObjectPropertyPolicyInput input = new SetObjectPropertyPolicyInput();
		ObjectPropertyPolicy_type0 policy = new ObjectPropertyPolicy_type0();
		PolicyType ptype; //  = new PolicyType();
		PolicyProperty property; //  = new PolicyProperty();
		Modifiers modifier = new Modifiers();
		
		modifier.setName("withProperties");
		modifier.setValue("true");
		
		ptype = new PolicyType();
		ptype.setName("GroupMember");
		property = new PolicyProperty();
		property.setName("the_role");
		property.addModifiers(modifier);
		ptype.addProperties(property);
		property = new PolicyProperty();
		property.setName("revision_list");
		property.addModifiers(modifier);
		ptype.addProperties(property);
		policy.addTypes(ptype);
		
		ptype = new PolicyType();
		ptype.setName("the_group");
		property.addModifiers(modifier);
		property = new PolicyProperty();
		property.setName("object_string");
		ptype.addProperties(property);
		policy.addTypes(ptype);
	
		ptype = new PolicyType();
		ptype.setName("Role");
		property.addModifiers(modifier);
		property = new PolicyProperty();
		property.setName("object_name");
		ptype.addProperties(property);
		property = new PolicyProperty();
		property.setName("description");
		ptype.addProperties(property);
		policy.addTypes(ptype);
		
		ptype = new PolicyType();
		ptype.setName("Group");
		property.addModifiers(modifier);
		property = new PolicyProperty();
		property.setName("object_name");
		ptype.addProperties(property);
		property = new PolicyProperty();
		property.setName("description");
		ptype.addProperties(property);
		
		policy.addTypes(ptype);

		input.setObjectPropertyPolicy(policy);
		
		try
		{
			session.setObjectPropertyPolicy(input);
		}
		catch (RemoteException e)
		{
			e.printStackTrace();
		}
		catch (com.teamcenter.services.core._2008_06.InternalServerFaultFault e)
		{
			e.printStackTrace();
		}
		catch (com.teamcenter.services.core._2008_06.InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}
	}
}
