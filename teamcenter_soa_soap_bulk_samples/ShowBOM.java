/* This example was tested with the java HelloTcSoap example provided in the soa_client.zip file.
It assumes you have the HelloTcSoap example working correctly. 

This example will demonstrate how to show a BOMWindow structure.
 
Add the following XML code to the HelloTcSoap Build.xml file (after other codegenwsdlfilename nodes) 
to auto-generate the needed java stub classes.
-----------------------------------------------------------------------------------------------
 
 <codegen wsdlfilename="${soa.home}/wsdls/Cad0701StructureManagementService.wsdl"
    	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
  <codegen wsdlfilename="${soa.home}/wsdls/Cad0706StructureManagementService.wsdl"
      	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
  <codegen wsdlfilename="${soa.home}/wsdls/Core0709DataManagementService.wsdl"
      	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
 
Add the following code the the Hello.java to call/use the custom java code.
---------------------------------------------------------------------------
 
	ShowBOM bom = new ShowBOM();
  		
  	if(bom.loadItem())
  		bom.showBOMWindowStructure();

	
Add the following class to package com.teamcenter.hellosoap.
------------------------------------------------------------ */

package com.teamcenter.hellosoap;

import java.math.BigInteger;
import java.rmi.RemoteException;
import java.util.Calendar;

import com.teamcenter.services.cad._2007_01.Cad0701StructureManagementService;
import com.teamcenter.services.cad._2007_01.ServiceFaultFault;
import com.teamcenter.services.cad._2007_06.Cad0706StructureManagementService;
import com.teamcenter.services.core._2007_09.Core0709DataManagementService;
import com.teamcenter.services.core._2007_09.InternalServerFaultFault;
import com.teamcenter.services.core._2007_09.InvalidUserFaultFault;
import com.teamcenter.services.core._2008_06.Core0806SessionService;

import com.teamcenter.schemas.cad._2007_01.structuremanagement.BOMLineFilter;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.CloseBOMWindowsInput;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.CreateBOMWindowsInfo;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.CreateBOMWindowsInput;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.CreateBOMWindowsResponse;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.CreateBOMWindowsOutput;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.ExpandPSData;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.ExpandPSOneLevelInfo;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.ExpandPSOneLevelInput;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.ExpandPSOneLevelOutput;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.ExpandPSOneLevelPref;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.ExpandPSOneLevelResponse;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.GetRevisionRulesInput;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.GetRevisionRulesResponse;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.GetVariantRulesInput;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.GetVariantRulesResponse;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.ItemRevisionToVariantRulesMap;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.RevisionRuleConfigInfo;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.RevisionRuleEntryProps;
import com.teamcenter.schemas.core._2007_09.datamanagement.LoadObjectsInput;
import com.teamcenter.schemas.core._2008_06.session.SetObjectPropertyPolicyInput;
import com.teamcenter.schemas.soa._2006_03.base.ModelObject;
import com.teamcenter.schemas.soa._2006_03.base.Modifiers;
import com.teamcenter.schemas.soa._2006_03.base.ObjectPropertyPolicy_type0;
import com.teamcenter.schemas.soa._2006_03.base.PolicyProperty;
import com.teamcenter.schemas.soa._2006_03.base.PolicyType;
import com.teamcenter.schemas.soa._2006_03.base.Property;
import com.teamcenter.schemas.soa._2006_03.base.PropertyValue;
import com.teamcenter.schemas.soa._2006_03.base.RefId;
import com.teamcenter.schemas.soa._2006_03.base.ServiceData;

public class ShowBOM
{
	private String ItemUID = "QGUhUOOl4aTB3C"; /*** You will need to update this UID to match your environment ***/

	private Core0709DataManagementService dmService;
	private Cad0706StructureManagementService structService706;
	private Cad0701StructureManagementService structService701;

	private ModelObject item;
	
	static final ModelObject NULL_OBJECT = new ModelObject();
    static
    {
        NULL_OBJECT.setUid("AAAAAAAAAAAAAA");
    }

	public ShowBOM()
	{
		dmService = (Core0709DataManagementService)Session.createServiceStub(Core0709DataManagementService.class);
		structService706 = (Cad0706StructureManagementService) Session.createServiceStub(Cad0706StructureManagementService.class);
		structService701 = (Cad0701StructureManagementService) Session.createServiceStub(Cad0701StructureManagementService.class);

		setObjectPolicy();
	}

	public boolean loadItem()
	{
		try
		{
			LoadObjectsInput input = new LoadObjectsInput();
			
			input.addUids(ItemUID);
			ServiceData sData = dmService.loadObjects(input);
			
			RefId[] ids = sData.getPlainObjs();
			
			if(ids != null && ids.length > 0)
			{
				ClientDataModel.addObjects(sData);
				ModelObject[] objs = ClientDataModel.getObjects(ids);
				
				for(ModelObject mObj : objs)
				{
					if(mObj.getClassName().equals("Item"))
					{
						item =  mObj;
		
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

	public boolean showBOMWindowStructure()
	{
		ModelObject[] bomViews = null;
		ModelObject[] itemRevs = null;
		
		try
		{
			bomViews = ClientDataModel.getPropertyModelObjectValues(item, "bom_view_tags");
			itemRevs = ClientDataModel.getPropertyModelObjectValues(item, "revision_list");
		}
		catch(NotLoadedException e1)
		{
			e1.printStackTrace();
		}
		
		if(itemRevs != null && bomViews != null)
		{
 			for(ModelObject bomView : bomViews)
			{
				for(ModelObject itemRev : itemRevs)
				{
					try
					{
						CreateBOMWindowsInfo bomWinInfo = new CreateBOMWindowsInfo();
						CreateBOMWindowsInput input = new CreateBOMWindowsInput();
						RevisionRuleConfigInfo ruleConfig = new RevisionRuleConfigInfo();
						RevisionRuleEntryProps ruleProp = new RevisionRuleEntryProps();
						
						ruleProp.setUnitNo(new BigInteger("0"));
						ruleProp.setDate(Calendar.getInstance());
						ruleProp.setEndItem(NULL_OBJECT);
						ruleProp.setEndItemRevision(NULL_OBJECT);
						ruleConfig.setRevRule(NULL_OBJECT); // getRevisionRule("Latest Working"));
						ruleConfig.setClientId(ruleConfig.toString());
						ruleConfig.setProps(ruleProp);
	
						bomWinInfo.setClientId(bomWinInfo.toString());
						bomWinInfo.setItem(item);
						bomWinInfo.setItemRev(itemRev);
						bomWinInfo.setBomView(bomView);
						bomWinInfo.setRevRuleConfigInfo(ruleConfig);
						bomWinInfo.setObjectForConfigure(NULL_OBJECT); // getVariantRules(itemRev));
						bomWinInfo.setActiveAssemblyArrangement(NULL_OBJECT);
						input.addInfo(bomWinInfo);
	
						CreateBOMWindowsResponse bomResp = structService701.createBOMWindows(input);
						
						ClientDataModel.addObjects(bomResp.getServiceData());
						
						CreateBOMWindowsOutput[] BOMWinOutput = bomResp.getOutput();
						
						if(BOMWinOutput != null)
						{
							for(CreateBOMWindowsOutput BOMWin : BOMWinOutput)
							{
								CloseBOMWindowsInput closeInput = new CloseBOMWindowsInput();
								
								expandBOMLines(BOMWin.getBomLine());
								// expandBOMAllLines(bomLine);
								
								closeInput.addBomWindows(BOMWin.getBomWindow());
								structService701.closeBOMWindows(closeInput);
							}
						}
					}
					catch (RemoteException e)
					{
						e.printStackTrace();
					}
					catch (com.teamcenter.services.cad._2007_01.InternalServerFaultFault e)
					{
						e.printStackTrace();
					}
					catch (com.teamcenter.services.cad._2007_01.InvalidUserFaultFault e)
					{
						e.printStackTrace();
					}
				}
			}

			return true; 
		}

		return false; 
	}
	
	protected ModelObject getRevisionRule(final String name)
	{	
		try
		{
			GetRevisionRulesInput ruleInput = new GetRevisionRulesInput();
			GetRevisionRulesResponse ruleResp = structService701.getRevisionRules(ruleInput);
			
			ClientDataModel.addObjects(ruleResp.getServiceData());
			
			// RevisionRuleInfo[] ruleInfo = ruleResp.getOutput(); // doesn't return the objects with properties.
			/* for(RevisionRuleInfo rule : ruleInfo)
			{
				Property[] properties = rule.getRevRule().getProperties();
				
				for(Property prop : properties)
				{					
					if(prop.getName().equals("object_name"))
					{
						 for(PropertyValue value : prop.getValues())
						 {
							 if(value.getValue().equals(name))
								 return rule.getRevRule();
						 }
					}
				}
			} */
			
			ModelObject[] allObjs = ruleResp.getServiceData().getDataObjects();
			
			for(ModelObject mObj : allObjs)
			{
				Property[] properties = mObj.getProperties();
				
				for(Property prop : properties)
				{					
					if(prop.getName().equals("object_name"))
					{
						 for(PropertyValue value : prop.getValues())
						 {
							 if(value.getValue().equals(name))
								 return mObj;
						 }
					}
				}
			}
		}
		catch (RemoteException e)
		{
			e.printStackTrace();
		}
		catch (com.teamcenter.services.cad._2007_01.InternalServerFaultFault e)
		{
			e.printStackTrace();
		}
		catch (ServiceFaultFault e)
		{
			e.printStackTrace();
		}
		catch (com.teamcenter.services.cad._2007_01.InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}  
		
		return null;
	}
	
	protected ModelObject getVariantRules(ModelObject itemRev)
	{
		try
		{
			GetVariantRulesInput ruleInput = new GetVariantRulesInput();
			
			ruleInput.setItemRevs(new ModelObject[]{ itemRev });
			GetVariantRulesResponse ruleResp = structService701.getVariantRules(ruleInput);
			
			ClientDataModel.addObjects(ruleResp.getServiceData());
			
			ItemRevisionToVariantRulesMap[] ruleMap = ruleResp.getInputItemRevToVarRules();
			
			for(ItemRevisionToVariantRulesMap rule : ruleMap)
			{
				ModelObject[] mObjs = rule.getValue();
				
				if(mObjs != null && mObjs.length > 0)
					return mObjs[0];
			}
		}
		catch (RemoteException e)
		{
			e.printStackTrace();
		}
		catch (com.teamcenter.services.cad._2007_01.InternalServerFaultFault e)
		{
			e.printStackTrace();
		}
		catch (ServiceFaultFault e)
		{
			e.printStackTrace();
		}
		catch (com.teamcenter.services.cad._2007_01.InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}
		
		return null;
	}

	private void expandBOMLines(final ModelObject bomLine)
	{
		try
		{
			ExpandPSOneLevelInput input = new ExpandPSOneLevelInput();
			ExpandPSOneLevelInfo levelInfo = new ExpandPSOneLevelInfo();
			ExpandPSOneLevelPref levelPref = new ExpandPSOneLevelPref();
			
			levelInfo.setParentBomLines(new ModelObject[]{ bomLine });
			levelInfo.setExcludeFilter(BOMLineFilter.None);
	
			levelPref.setExpItemRev(true);
			
			input.setInput(levelInfo);
			input.setPref(levelPref);
		
			ExpandPSOneLevelResponse levelResp;
		
			levelResp = structService701.expandPSOneLevel(input);
			
			ClientDataModel.addObjects(levelResp.getServiceData());
			
			ModelObject[] mObjs = levelResp.getServiceData().getDataObjects();
			
			if(mObjs == null)
				return;
			
			for(ModelObject mObj : mObjs)
			{
				if(mObj.getClassName().equals("BOMLine"))
				{
					for(Property prop : mObj.getProperties())
					{					
						if(prop.getName().equals("bl_line_name"))
							for(PropertyValue value : prop.getValues())
								 System.out.println("BOMLine Name: " + value.getValue());
					}
				}
			}
			
			ExpandPSOneLevelOutput[] levelOut = levelResp.getOutput();
			
			for(ExpandPSOneLevelOutput level : levelOut)
			{				
				for(ExpandPSData data : level.getChildren())
				{
					expandBOMLines(data.getBomLine());
				}
			}
		}
		catch (RemoteException e)
		{
			e.printStackTrace();
		}
		catch (com.teamcenter.services.cad._2007_01.InternalServerFaultFault e)
		{
			e.printStackTrace();
		}
		catch (com.teamcenter.services.cad._2007_01.InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}
	}

	/* private void expandBOMAllLines(final BOMLine bomLine)
	{
		com.teamcenter.services.strong.cad._2007_01.StructureManagement.ExpandPSAllLevelsInfo info = new com.teamcenter.services.strong.cad._2007_01.StructureManagement.ExpandPSAllLevelsInfo();
		com.teamcenter.services.strong.cad._2007_01.StructureManagement.ExpandPSAllLevelsPref pref = new com.teamcenter.services.strong.cad._2007_01.StructureManagement.ExpandPSAllLevelsPref();

		info.parentBomLines = new BOMLine[]{ bomLine };
		info.excludeFilter = "None";

		pref.expItemRev = false;
		pref.info = new com.teamcenter.services.strong.cad._2007_01.StructureManagement.RelationAndTypesFilter[0];

		com.teamcenter.services.strong.cad._2007_01.StructureManagement.ExpandPSAllLevelsResponse resp = structureService.expandPSAllLevels(info, pref);

		for(com.teamcenter.services.strong.cad._2007_01.StructureManagement.ExpandPSAllLevelsOutput data : resp.output)
		{
			try
			{
				System.out.println("Parent: " + data.parent.bomLine.get_bl_line_name());

				if(data.children.length > 0)
				{
					for(com.teamcenter.services.strong.cad._2007_01.StructureManagement.ExpandPSData child : data.children)
						System.out.println("\tChild: " + child.bomLine.get_bl_line_name());
				}
				else
					System.out.println("\tChildren: none");
			}
			catch(NotLoadedException e)
			{
				e.printStackTrace();
			}
		}
	} */

	protected void setObjectPolicy()
	{
		try
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
			ptype.setName("RevisionRule");
			property = new PolicyProperty();
			property.setName("object_name");
			ptype.addProperties(property);
			policy.addTypes(ptype);
			
			ptype = new PolicyType();
			ptype.setName("Item");
			property = new PolicyProperty();
			property.setName("bom_view_tags");
			property.addModifiers(modifier);
			ptype.addProperties(property);
			property = new PolicyProperty();
			property.setName("revision_list");
			property.addModifiers(modifier);
			ptype.addProperties(property);
			policy.addTypes(ptype);
			
			ptype = new PolicyType();
			ptype.setName("BOMLine");
			property = new PolicyProperty();
			property.setName("bl_line_name");
			ptype.addProperties(property);
			policy.addTypes(ptype);
			
			ptype = new PolicyType();
			ptype.setName("BOMWindow");
			property = new PolicyProperty();
			property.setName("is_packed_by_default");
			ptype.addProperties(property);
			policy.addTypes(ptype);
			
			ptype = new PolicyType();
			ptype.setName("RevisionRule");
			property = new PolicyProperty();
			property.setName("object_name");
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
