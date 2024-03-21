/* This example was tested with the java HelloTcSoap example provided in the soa_client.zip file.
It assumes you have the HelloTcSoap example working correctly. 

This example will demonstrate how to add a BOMline to the TopLine (BOMline).
 
Add the following XML code to the HelloTcSoap Build.xml file (after other codegenwsdlfilename nodes) 
to auto-generate the needed java stub classes.
-----------------------------------------------------------------------------------------------
 
 <codegen wsdlfilename="${soa.home}/wsdls/Bom0806StructureManagementService.wsdl"
    	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
  <codegen wsdlfilename="${soa.home}/wsdls/Query0706FinderService.wsdl"
      	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
  <codegen wsdlfilename="${soa.home}/wsdls/Cad0806StructureManagementService.wsdl"
      	    			output="${axis.out}" serverSide="false" unpackClasses="true" />
 
Add the following code the the Hello.java to call/use the custom java code.
---------------------------------------------------------------------------
 
	AddBOMLineToParent bom = new AddBOMLineToParent();
  		
  	if(bom.loadObjects())
  		bom.updateStructure();

	
Add the following class to package com.teamcenter.hellosoap.
------------------------------------------------------------ */

package com.teamcenter.hellosoap;

import java.util.Calendar;
import java.util.Vector;
import java.math.BigInteger;
import java.rmi.RemoteException;

import com.teamcenter.schemas.bom._2008_06.structuremanagement.AddOrUpdateChildrenToParentLineInfo;
import com.teamcenter.schemas.bom._2008_06.structuremanagement.AddOrUpdateChildrenToParentLineInput;
import com.teamcenter.schemas.bom._2008_06.structuremanagement.AddOrUpdateChildrenToParentLineResponse;
import com.teamcenter.schemas.bom._2008_06.structuremanagement.ItemLineInfo;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.CreateBOMWindowsInfo;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.CreateBOMWindowsInput;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.CreateBOMWindowsOutput;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.CreateBOMWindowsResponse;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.RevisionRuleConfigInfo;
import com.teamcenter.schemas.cad._2007_01.structuremanagement.RevisionRuleEntryProps;
import com.teamcenter.schemas.cad._2008_06.structuremanagement.SaveBOMWindowsInput;
import com.teamcenter.schemas.cad._2008_06.structuremanagement.SaveBOMWindowsResponse;
import com.teamcenter.schemas.core._2007_09.datamanagement.LoadObjectsInput;
import com.teamcenter.schemas.core._2008_06.session.SetObjectPropertyPolicyInput;
import com.teamcenter.schemas.query._2007_06.finder.FindWorkspaceObjectsInput;
import com.teamcenter.schemas.query._2007_06.finder.FindWorkspaceObjectsResponse;
import com.teamcenter.schemas.query._2007_06.finder.WSOFindCriteria;
import com.teamcenter.schemas.query._2007_06.finder.WSOFindSet;
import com.teamcenter.schemas.query._2007_06.finder.WSOscopeType;
import com.teamcenter.schemas.soa._2006_03.base.ErrorStack;
import com.teamcenter.schemas.soa._2006_03.base.ErrorValue;
import com.teamcenter.schemas.soa._2006_03.base.ModelObject;
import com.teamcenter.schemas.soa._2006_03.base.Modifiers;
import com.teamcenter.schemas.soa._2006_03.base.ObjectPropertyPolicy_type0;
import com.teamcenter.schemas.soa._2006_03.base.PolicyProperty;
import com.teamcenter.schemas.soa._2006_03.base.PolicyType;
import com.teamcenter.schemas.soa._2006_03.base.RefId;
import com.teamcenter.schemas.soa._2006_03.base.ServiceData;
import com.teamcenter.services.bom._2008_06.Bom0806StructureManagementService;
import com.teamcenter.services.cad._2007_01.Cad0701StructureManagementService;
import com.teamcenter.services.cad._2008_06.Cad0806StructureManagementService;
import com.teamcenter.services.core._2007_09.Core0709DataManagementService;
import com.teamcenter.services.core._2007_09.InternalServerFaultFault;
import com.teamcenter.services.core._2007_09.InvalidUserFaultFault;
import com.teamcenter.services.core._2008_06.Core0806SessionService;
import com.teamcenter.services.query._2007_06.Query0706FinderService;


public class AddBOMLineToParent
{
	private Core0709DataManagementService dmService;
	private	Bom0806StructureManagementService bomService;
	private Cad0701StructureManagementService structService701;
	private Cad0806StructureManagementService structService806;
	private Query0706FinderService finderService706;
	
	private ModelObject parentItem = null;
	private ModelObject parentRev = null;
	private ModelObject childItem = null;
	private ModelObject childRev = null;
	private ModelObject revisionRule = null;
	
	static final String parentItemUID = "QGUhUOOl4aTB3C"; /*** You will need to update these UIDs to match your environment ***/
	static final String parentRevUID = "QGchUOOl4aTB3C";
	static final String childItemUID = "AqbtEtU34aTB3C";
	static final String childRevUID = "AuStEtU34aTB3C";
		
	static final ModelObject NULL_OBJECT = new ModelObject();
	static final Calendar NULL_DATE = Calendar.getInstance();
    static
    {
        NULL_OBJECT.setUid("AAAAAAAAAAAAAA");
        NULL_DATE.set(1, 0, 1, 0, 0, 0);
    }
    
	public AddBOMLineToParent()
	{
		dmService = (Core0709DataManagementService)Session.createServiceStub(Core0709DataManagementService.class);
		bomService = (Bom0806StructureManagementService) Session.createServiceStub(Bom0806StructureManagementService.class);
		structService701 = (Cad0701StructureManagementService) Session.createServiceStub(Cad0701StructureManagementService.class);
		structService806 = (Cad0806StructureManagementService) Session.createServiceStub(Cad0806StructureManagementService.class);
		finderService706 = (Query0706FinderService) Session.createServiceStub(Query0706FinderService.class);

		setObjectPolicy();
	}
	
	public boolean loadObjects()
	{
		Vector<Boolean> successList = new Vector<Boolean>();
		
		revisionRule = getWorkspaceObject("Latest Working", "RevisionRule");
		
		if(revisionRule == null)
			return false;
		
		try
		{	
			LoadObjectsInput input = new LoadObjectsInput();
			
			input.addUids(parentItemUID);
			input.addUids(parentRevUID);
						
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
						parentItem =  mObj;
						successList.add(true);
					}
					else if(mObj.getClassName().equals("ItemRevision"))
					{
						parentRev =  mObj;
						successList.add(true);
					}
					else 
						return false;
				}
			}
			
			input = new LoadObjectsInput();
			input.addUids(childItemUID);
			input.addUids(childRevUID);
			sData = dmService.loadObjects(input);
			ids = sData.getPlainObjs();
			
			if(ids != null && ids.length > 0)
			{
				ClientDataModel.addObjects(sData);
				ModelObject[] objs = ClientDataModel.getObjects(ids);
				
				for(ModelObject mObj : objs)
				{
					if(mObj.getClassName().equals("Item"))
					{
						childItem =  mObj;
						successList.add(true);
					}
					else if(mObj.getClassName().equals("ItemRevision"))
					{
						childRev =  mObj;
						successList.add(true);
					}
					else
						return false;
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

		return successList.size() == 4;
	}
	
	protected ModelObject getWorkspaceObject(final String name, final String typeName)
	{
		try
		{
			WSOFindCriteria criteria = new WSOFindCriteria();
			WSOFindSet set = new WSOFindSet();
			FindWorkspaceObjectsInput input = new FindWorkspaceObjectsInput();

			criteria.setObjectName(name);
			criteria.setObjectType(typeName);
			criteria.setScope(WSOscopeType.WSO_scope_All);
			criteria.setOwner(NULL_OBJECT);
			criteria.setGroup(NULL_OBJECT);
			criteria.setCreatedBefore(NULL_DATE);
			criteria.setCreatedAfter(NULL_DATE);
			criteria.setModifiedAfter(NULL_DATE);
			criteria.setModifiedBefore(NULL_DATE);
			criteria.setReleasedAfter(NULL_DATE);
			criteria.setReleasedBefore(NULL_DATE);
			
			set.setCriterias(new WSOFindCriteria[]{ criteria });
			input.setFindList(new WSOFindSet[]{ set });

			FindWorkspaceObjectsResponse resp = finderService706.findWorkspaceObjects(input);

			if(!ServiceDataError(resp.getServiceData()) && resp.getOutputList().length > 0)
			{
				return resp.getOutputList()[0].getFoundObjects()[0];
			}
		}
		catch(RemoteException e)
		{
			e.printStackTrace();
		}
		catch(com.teamcenter.services.query._2007_06.InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}
		catch(com.teamcenter.services.query._2007_06.ServiceFaultFault e)
		{
			e.printStackTrace();
		}
		catch(com.teamcenter.services.query._2007_06.InternalServerFaultFault e)
		{
			e.printStackTrace();
		}
		
		return null;
	}
	
    public boolean updateStructure() 
    {
        RevisionRuleConfigInfo ruleConfig = new RevisionRuleConfigInfo();
        RevisionRuleEntryProps ruleProp = new RevisionRuleEntryProps();
        CreateBOMWindowsInfo bomWinInfo = new CreateBOMWindowsInfo();
        CreateBOMWindowsInput createBOMWindowsInput = new CreateBOMWindowsInput();
        
        ruleProp.setUnitNo(new BigInteger("-1"));
        ruleProp.setToday(true);
        ruleProp.setDate(Calendar.getInstance());
        ruleProp.setEndItem(parentItem);                                                  
        ruleProp.setEndItemRevision(parentRev);
        
        ruleConfig.setClientId(ruleConfig.toString());
        ruleConfig.setRevRule(revisionRule);
        ruleConfig.setProps(ruleProp);
       
        bomWinInfo.setClientId(bomWinInfo.toString());
        bomWinInfo.setItem(parentItem);
        bomWinInfo.setItemRev(parentRev);
        bomWinInfo.setRevRuleConfigInfo(ruleConfig);                                                           
        bomWinInfo.setObjectForConfigure(revisionRule);   
        bomWinInfo.setActiveAssemblyArrangement(parentRev);
        bomWinInfo.setBomView(NULL_OBJECT);
        
        createBOMWindowsInput.addInfo(bomWinInfo);
        
        try 
        {
            CreateBOMWindowsResponse resp = structService701.createBOMWindows(createBOMWindowsInput);
                       
            if(!ServiceDataError(resp.getServiceData()) && resp.getOutput().length > 0)
            {
            	AddOrUpdateChildrenToParentLineInfo parentLineInfo = new AddOrUpdateChildrenToParentLineInfo();
 	            AddOrUpdateChildrenToParentLineInput parentLineInput = new AddOrUpdateChildrenToParentLineInput();
 	            ItemLineInfo lineInfo = new ItemLineInfo();
 	            
 	            ClientDataModel.addObjects(resp.getServiceData());
            	CreateBOMWindowsOutput[] output = resp.getOutput();
	            ModelObject bomWindow = output[0].getBomWindow();
	            ModelObject bomLine = output[0].getBomLine(); // Topline

	            lineInfo.setClientId(bomWinInfo.toString());
	            lineInfo.setItem(NULL_OBJECT);
	            lineInfo.setItemRev(childRev);
	            lineInfo.setOccType("MEOther");
	            lineInfo.setBomline(NULL_OBJECT);
	           
	            parentLineInfo.setItems(new ItemLineInfo[]{ lineInfo });
	            parentLineInfo.setParentLine(bomLine);
	            parentLineInfo.setViewType("view");
	            
	            parentLineInput.addInputs(parentLineInfo);
	            
	            AddOrUpdateChildrenToParentLineResponse resp2 = bomService.addOrUpdateChildrenToParentLine(parentLineInput);
	            
	            if(!ServiceDataError(resp2.getServiceData()))
	            {
	            	SaveBOMWindowsInput input = new SaveBOMWindowsInput();
	            	
	            	input.setBomWindows(new ModelObject[]{ bomWindow });
	            	SaveBOMWindowsResponse resp3 = structService806.saveBOMWindows(input);
	            	
	            	if(!ServiceDataError(resp3.getServiceData()))
	            		return true;
	            }
            }
        }
        catch(com.teamcenter.services.cad._2007_01.InvalidUserFaultFault e) 
        {
            e.printStackTrace();
        }
        catch(com.teamcenter.services.cad._2007_01.InternalServerFaultFault e) 
        {
        	e.printStackTrace();
        }
        catch(com.teamcenter.services.bom._2008_06.InvalidUserFaultFault e) 
        {
        	e.printStackTrace();
        }
        catch(com.teamcenter.services.bom._2008_06.InternalServerFaultFault e) 
        {
        	e.printStackTrace();
        }
        catch(RemoteException e) 
        {
            e.printStackTrace();
        }
		catch(com.teamcenter.services.cad._2008_06.InvalidUserFaultFault e)
		{	
			e.printStackTrace();
		}
		catch(com.teamcenter.services.cad._2008_06.InternalServerFaultFault e)
		{
			e.printStackTrace();
		}  
        
        return false;
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
		try
		{
			Core0806SessionService session = (Core0806SessionService)Session.createServiceStub(Core0806SessionService.class);
			SetObjectPropertyPolicyInput input = new SetObjectPropertyPolicyInput();
			ObjectPropertyPolicy_type0 policy = new ObjectPropertyPolicy_type0();
			PolicyType ptype;
			PolicyProperty property;
			Modifiers modifier = new Modifiers();
			
			modifier.setName("withProperties");
			modifier.setValue("true");

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
		catch (com.teamcenter.services.core._2008_06.InternalServerFaultFault e)
		{
			e.printStackTrace();
		}
		catch (com.teamcenter.services.core._2008_06.InvalidUserFaultFault e)
		{
			e.printStackTrace();
		}
		catch (RemoteException e)
		{
			e.printStackTrace();
		}
	}
}
