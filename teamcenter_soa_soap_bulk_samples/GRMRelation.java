/* This example was tested with the java HelloTcSoap example provided in the soa_client.zip file.
It assumes you have the HelloTcSoap example working correctly. 

This example will demonstrate how to seconday_object in a GRM relation.
 
Add the following XML code to the HelloTcSoap Build.xml file (after other codegenwsdlfilename nodes) 
to auto-generate the needed java stub classes.
-----------------------------------------------------------------------------------------------
 
 <codegen wsdlfilename="${soa.home}/wsdls/Core0709DataManagementService.wsdl"
    	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
  
Add the following code the the Hello.java to call/use the custom java code.
---------------------------------------------------------------------------
 
	GRMRelation gm = new GRMRelation();
  		
  	if(gm.loadItem())
  		gm.getGRMRelation();

	
Add the following class to package com.teamcenter.hellosoap.
------------------------------------------------------------ */

package com.teamcenter.hellosoap;

import java.rmi.RemoteException;

import com.teamcenter.schemas.core._2007_06.datamanagement.RelationAndTypesFilter;
import com.teamcenter.schemas.core._2007_09.datamanagement.ExpandGRMRelationsForPrimaryInput;
import com.teamcenter.schemas.core._2007_09.datamanagement.ExpandGRMRelationsPref2;
import com.teamcenter.schemas.core._2007_09.datamanagement.ExpandGRMRelationsResponse2;
import com.teamcenter.schemas.core._2007_09.datamanagement.LoadObjectsInput;
import com.teamcenter.schemas.core._2008_06.session.SetObjectPropertyPolicyInput;
import com.teamcenter.schemas.soa._2006_03.base.ModelObject;
import com.teamcenter.schemas.soa._2006_03.base.ObjectPropertyPolicy_type0;
import com.teamcenter.schemas.soa._2006_03.base.PolicyProperty;
import com.teamcenter.schemas.soa._2006_03.base.PolicyType;
import com.teamcenter.schemas.soa._2006_03.base.Property;
import com.teamcenter.schemas.soa._2006_03.base.PropertyValue;
import com.teamcenter.schemas.soa._2006_03.base.RefId;
import com.teamcenter.schemas.soa._2006_03.base.ServiceData;
import com.teamcenter.services.core._2007_09.Core0709DataManagementService;
import com.teamcenter.services.core._2007_09.InternalServerFaultFault;
import com.teamcenter.services.core._2007_09.InvalidUserFaultFault;
import com.teamcenter.services.core._2008_06.Core0806SessionService;

public class GRMRelation
{
	private String ItemRevUID = "Q5YBc2n4IJNDJD"; /*** You will need to update this UID to match your environment ***/
	private Core0709DataManagementService dmService;
	private ModelObject itemRev;

	public GRMRelation()
	{
		dmService = (Core0709DataManagementService)Session.createServiceStub(Core0709DataManagementService.class);

		setObjectPolicy();
	}
	
	public boolean loadItem()
	{
		try
		{
			LoadObjectsInput input = new LoadObjectsInput();
			
			input.addUids(ItemRevUID);
			ServiceData sData = dmService.loadObjects(input);
			
			RefId[] ids = sData.getPlainObjs();
			
			if(ids != null && ids.length > 0)
			{
				ClientDataModel.addObjects(sData);
				ModelObject[] objs = ClientDataModel.getObjects(ids);
				
				for(ModelObject mObj : objs)
				{
					if(mObj.getClassName().equals("ItemRevision"))
					{
						itemRev =  mObj;
						return true;
					}
				}
			}
		}
		catch (RemoteException e)
		{
			e.printStackTrace();
		}
		catch (InternalServerFaultFault e)
		{
			e.printStackTrace();
		}
		catch (InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}

		return false;
	}
	
	void getGRMRelation()
	{
		try
		{
			RelationAndTypesFilter filter = new RelationAndTypesFilter();
			ExpandGRMRelationsPref2 pref = new ExpandGRMRelationsPref2();
			ExpandGRMRelationsForPrimaryInput input = new ExpandGRMRelationsForPrimaryInput();
			
			filter.setRelationTypeName("TC_WorkContext_Relation"); // OOTB ImanRelation name, update to specific relation of interest.
			pref.addInfo(filter);
			input.addPrimaryObjects(itemRev);
			input.setPref(pref);
			
			ExpandGRMRelationsResponse2 resp = dmService.expandGRMRelationsForPrimary(input);
			
			ModelObject[] mObjs = resp.getServiceData().getDataObjects();
			
			if(mObjs != null && mObjs.length > 0 )
			{
				for(ModelObject mObj : mObjs)
				{
					String name = mObj.getClassName();
					System.out.println("ClassName: " + name);
					
					if(name.equals("TC_WorkContext")) // typically, the 'secondary_object' ImanType name.
					{
						Property[] properties = mObj.getProperties();
						
						if(properties != null)
						{
							for(Property prop : properties)
							{
								System.out.print(prop.getName());
								
								for(PropertyValue value : prop.getValues())
								{
									System.out.print(":" + value.getValue());
								}
								
								System.out.println();
							}
						}
					}
				}
			}
		}
		catch (RemoteException e)
		{
			e.printStackTrace();
		}
		catch (InternalServerFaultFault e)
		{
			e.printStackTrace();
		}
		catch (InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}
	}
	
	protected void setObjectPolicy()
	{
		try
		{
			Core0806SessionService session = (Core0806SessionService)Session.createServiceStub(Core0806SessionService.class);
			SetObjectPropertyPolicyInput input = new SetObjectPropertyPolicyInput();
			ObjectPropertyPolicy_type0 policy = new ObjectPropertyPolicy_type0();
			PolicyType ptype; 
			PolicyProperty property;

			ptype = new PolicyType();
			ptype.setName("TC_WorkContext");
			property = new PolicyProperty();
			property.setName("workcontext_desc");
			ptype.addProperties(property);
			property = new PolicyProperty();
			property.setName("workcontext_name");
			ptype.addProperties(property);
			policy.addTypes(ptype);
			
			input.setObjectPropertyPolicy(policy);
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
