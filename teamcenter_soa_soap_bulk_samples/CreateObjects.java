/* This example was tested with the java HelloTcSoap example provided in the soa_client.zip file.
It assumes you have the HelloTcSoap example working correctly. 

This example will demonstrate how to create a Item/ItemRevision business objects..
 
Add the following XML code to the HelloTcSoap Build.xml file (after other codegenwsdlfilename nodes) 
to auto-generate the needed java stub classes.
-----------------------------------------------------------------------------------------------
 
 <codegen wsdlfilename="${soa.home}/wsdls/Core0806DataManagementService.wsdl"
    	    			output="${axis.out}" serverSide="false" unpackClasses="true" />

 
Add the following code the the Hello.java to call/use the custom java code.
---------------------------------------------------------------------------
 
	CreateObjects co = new CreateObjects();
  		
	co.createObject();

	
Add the following class to package com.teamcenter.hellosoap.
------------------------------------------------------------ */

package com.teamcenter.hellosoap;

import java.rmi.RemoteException;


import com.teamcenter.schemas.core._2008_06.datamanagement.CreateIn;
import com.teamcenter.schemas.core._2008_06.datamanagement.CreateInput;
import com.teamcenter.schemas.core._2008_06.datamanagement.CreateInputMap;
import com.teamcenter.schemas.core._2008_06.datamanagement.CreateObjectsInput;
import com.teamcenter.schemas.core._2008_06.datamanagement.CreateResponse;
import com.teamcenter.schemas.core._2008_06.datamanagement.StringMap;
import com.teamcenter.schemas.soa._2006_03.base.ModelObject;
import com.teamcenter.services.core._2008_06.Core0806DataManagementService;
import com.teamcenter.services.core._2008_06.InternalServerFaultFault;
import com.teamcenter.services.core._2008_06.InvalidUserFaultFault;
import com.teamcenter.services.core._2008_06.ServiceFaultFault;

public class CreateObjects
{
	private Core0806DataManagementService dmService;
	
	public CreateObjects()
	{
		dmService = (Core0806DataManagementService)Session.createServiceStub(Core0806DataManagementService.class);
	}
	
	public void createObject()
	{
		try
		{
			CreateInput itemDef = new CreateInput();
			CreateInput itemRevDef = new CreateInput();
			
			StringMap propStringMap[] = new StringMap[2];
			itemRevDef.setBoName("ItemRevision");
			propStringMap = new StringMap[2];
			propStringMap[0] = new StringMap();
			propStringMap[0].setKey("object_name");
			propStringMap[0].setValue("ItemRev (object_name)");
			propStringMap[1] = new StringMap();
			propStringMap[1].setKey("object_desc");
			propStringMap[1].setValue("ItemRev (object_desc)");

			itemDef.setBoName("Item");
			
			CreateInputMap inMap = new CreateInputMap();
			inMap.setKey("revision");
			inMap.addValue(itemRevDef);
			itemDef.setCompoundCreateInput(new CreateInputMap[] { inMap });
			propStringMap[0] = new StringMap();
			propStringMap[0].setKey("object_name");
			propStringMap[0].setValue("Item (object_name)");
			propStringMap[1] = new StringMap();
			propStringMap[1].setKey("object_desc");
			propStringMap[1].setValue("Item (object_desc)");
			
			itemDef.setStringProps(propStringMap);	
			
			CreateIn in = new CreateIn();
			in.setClientId(in.toString());
			in.setData(itemDef);
			
			CreateObjectsInput objects = new CreateObjectsInput();
			objects.addInput(in);
			
			CreateResponse  resp = dmService.createObjects(objects);
			
			for(ModelObject obj : resp.getServiceData().getDataObjects())
			{
				System.out.println("UID: " + obj.getUid());
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
		catch (ServiceFaultFault e)
		{
			e.printStackTrace();
		}
		catch (InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}
	}
}
