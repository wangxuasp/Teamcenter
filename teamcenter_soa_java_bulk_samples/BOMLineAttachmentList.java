/*
This SOA java example will demonstrate how to reterive BOMLine.bl_attachments list.

The HelloTeamcenter java example was used to test this example (found in, soa_client.zip)

Update 'ItemUID' to the UID of your Item that can be used to create a BOMWindow and will have BOMLines with their  bl_attachments populated.

The class can be used as follows:


        BOMLineAttachmentList bltest = new BOMLineAttachmentList();
        
        if(bltest.loadItem())
        {
        	if(bltest.createBOMWindowStructure())
        	{
        		bltest.printBOMLineAttachments();
        		bltest.closeBOMWindows();
        	}
        }

*/

package com.teamcenter.hello;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement.CreateBOMWindowsResponse;
import com.teamcenter.services.strong.cad._2008_06.StructureManagement.ExpandPSData;
import com.teamcenter.services.strong.cad._2008_06.StructureManagement.ExpandPSOneLevelInfo;
import com.teamcenter.services.strong.cad._2008_06.StructureManagement.ExpandPSOneLevelOutput;
import com.teamcenter.services.strong.cad._2008_06.StructureManagement.ExpandPSOneLevelPref;
import com.teamcenter.services.strong.cad._2008_06.StructureManagement.ExpandPSOneLevelResponse2;
import com.teamcenter.services.strong.cad._2008_06.StructureManagement.RelationAndTypesFilter;
import com.teamcenter.services.strong.cad._2019_06.StructureManagement.CreateWindowsInfo3;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core.SessionService;
import com.teamcenter.services.strong.manufacturing._2009_06.StructureManagement.GetBOMLineAttachmentsResponse;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.BOMLine;
import com.teamcenter.soa.client.model.strong.BOMWindow;
import com.teamcenter.soa.client.model.strong.Item;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.soa.client.model.strong.PSBOMView;
import com.teamcenter.soa.client.model.strong.CfgAttachmentLine;
import com.teamcenter.soa.common.ObjectPropertyPolicy;
import com.teamcenter.soa.common.PolicyProperty;
import com.teamcenter.soa.common.PolicyType;
import com.teamcenter.soa.exceptions.NotLoadedException;

public class BOMLineAttachmentList 
{
	private DataManagementService dmService;
	private com.teamcenter.services.strong.cad.StructureManagementService structureService;
	private com.teamcenter.services.strong.manufacturing.StructureManagementService smService;
	
	private Item item;
	private BOMLine topLine;
	private BOMWindow bomWindow; 
	
	static final String ItemUID = "DfsN1rLjJCa5MB"; // Update with your Item UID
	
	
	public BOMLineAttachmentList()
	{
		dmService = DataManagementService.getService(AppXSession.getConnection());
		structureService = com.teamcenter.services.strong.cad.StructureManagementService.getService(AppXSession.getConnection());
		smService = com.teamcenter.services.strong.manufacturing.StructureManagementService.getService(AppXSession.getConnection());
		
		setObjectPolicy();
	}
	
	public boolean loadItem()
	{
		ServiceData sData = dmService.loadObjects(new String[]{ ItemUID }); // load object via UID.

		if(sData.sizeOfPlainObjects() > 0)
		{
			ModelObject mObj = sData.getPlainObject(0);

			if(mObj instanceof Item)
			{
				item = (Item) mObj;
				
				return true;
			}
		}

		return false;
	}
	
	boolean createBOMWindowStructure()
	{
		try
		{
			ModelObject[] itemRevs = item.get_revision_list();
			ModelObject[] bomViews = item.get_bom_view_tags();

			for(ModelObject bomView : bomViews)
			{
				for(ModelObject itemRev : itemRevs)
				{
					CreateWindowsInfo3 bomWinInfo = new CreateWindowsInfo3();

					bomWinInfo.item = item;
					bomWinInfo.itemRev = (ItemRevision) itemRev;
					bomWinInfo.bomView = (PSBOMView) bomView;

					CreateBOMWindowsResponse bomResp = structureService.createOrReConfigureBOMWindows(new CreateWindowsInfo3[]{ bomWinInfo });

					if(!ServiceDataError(bomResp.serviceData) && bomResp.output.length > 0)
					{
						topLine = bomResp.output[0].bomLine;
						bomWindow = bomResp.output[0].bomWindow;
						
						return true;
					}
				}
			}
		}
		catch(NotLoadedException e)
		{
			e.printStackTrace();
		}

		return false;
	}
	
	public void closeBOMWindows()
	{
		structureService.closeBOMWindows(new BOMWindow[]{ bomWindow });
	}
	
	public void printBOMLineAttachments()
	{
		Map<BOMLine, Vector<BOMLine>> bomMap = new HashMap<BOMLine, Vector<BOMLine>>();
		
		expandBOMLines(topLine, bomMap);
		
		try
		{
			BOMLine[] keyLines = bomMap.keySet().toArray(new BOMLine[0]);
			
			performCfgAttachmentLineOperation(keyLines);
		}
		catch (NotLoadedException e)
		{
			e.printStackTrace();
		}
	}
	
	void performCfgAttachmentLineOperation(BOMLine[] bomlines) throws NotLoadedException
	{
		GetBOMLineAttachmentsResponse resp = smService.getBOMLineAttachments(bomlines, new String[0]);
		
		if(!ServiceDataError(resp.serviceData))
		{
			for(Map.Entry<BOMLine, CfgAttachmentLine[]> entry : resp.lines.entrySet())
			{
				System.out.println("BOMLine: " + entry.getKey().get_bl_line_name());
				
				for(CfgAttachmentLine cfgline : entry.getValue()) // work on the entire CfgAttachmentLine list.
				{
					System.out.println("CfgAttachmentLine: " + cfgline.get_me_cl_object_string());
				}
			}
		
			ServiceData sdata = smService.closeAttachmentWindow(bomlines); // test sdata for errors
			
			ServiceDataError(sdata);
		}
	}
	
	private void expandBOMLines(final BOMLine bomLine, Map<BOMLine, Vector<BOMLine>> bomMap)
	{
		ExpandPSOneLevelInfo levelInfo = new ExpandPSOneLevelInfo();
		ExpandPSOneLevelPref levelPref = new ExpandPSOneLevelPref();

		levelInfo.parentBomLines = new BOMLine[]{ bomLine };
		levelInfo.excludeFilter = "None2";
		levelPref.expItemRev = true;
		RelationAndTypesFilter typeFilter = new RelationAndTypesFilter();
		
		typeFilter.namedRefHandler = "NoNamedRefs";
		levelPref.info = new RelationAndTypesFilter[]{ typeFilter };

		ExpandPSOneLevelResponse2 levelResp = structureService.expandPSOneLevel(levelInfo, levelPref);
		
		if(!ServiceDataError(levelResp.serviceData))
		{
			if(levelResp.output.length > 0)
			{
				bomMap.put(bomLine, new Vector<BOMLine>());
				
				for(ExpandPSOneLevelOutput levelOut : levelResp.output)
				{
					for(ExpandPSData psData : levelOut.children)
					{
						bomMap.get(bomLine).add(psData.bomLine);
						expandBOMLines(psData.bomLine, bomMap); // recursive call to be child BOMLines
					}
				}
			}
		}
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
	
	protected void setObjectPolicy()
	{
		SessionService session = SessionService.getService(AppXSession.getConnection());
		ObjectPropertyPolicy policy = new ObjectPropertyPolicy();

		PolicyType bomLineType = new PolicyType("BOMLine", new String[]{ "bl_line_name", "bl_item_object_type", "bl_attachments", "bl_rev_object_type" }, new String[] { PolicyProperty.WITH_PROPERTIES });
		
		policy.addType(bomLineType);
		policy.addType("Item", new String[]{ "bom_view_tags", "revision_list" });
		policy.addType("BOMWindow", new String[]{ "is_packed_by_default" });
		policy.addType("CfgAttachmentLine", new String[]{ "me_cl_object_string" });

		session.setObjectPropertyPolicy(policy);
	}
}
