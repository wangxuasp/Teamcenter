/*
This SOA java example will demonstrate how to use VariantManagementService.getBOMVariantRules.

The HelloTeamcenter java example was used to test this example (found in, soa_client.zip)

Update 'itemUID' to the UID of your BOM stucture root Item.

The class can be used as follows:

	VariantManagement vm = new VariantManagement(user, AppXSession.getConnection());
        
	if(vm.loadObjects())
        vm.listBOMVariantRulesFromBOMWindow();
*/


package com.teamcenter.hello;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.schemas.soa._2006_03.exceptions.ServiceException;
import com.teamcenter.services.strong.cad.StructureManagementService;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement.CreateBOMWindowsOutput;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core.SessionService;
import com.teamcenter.services.strong.structuremanagement.VariantManagementService;
import com.teamcenter.services.strong.structuremanagement._2013_05.VariantManagement.BOMVariantOptionValueEntry;
import com.teamcenter.services.strong.structuremanagement._2013_05.VariantManagement.BOMVariantRuleContents;
import com.teamcenter.services.strong.structuremanagement._2013_05.VariantManagement.BOMVariantRuleOutput;
import com.teamcenter.services.strong.structuremanagement._2013_05.VariantManagement.BOMVariantRulesResponse;
import com.teamcenter.services.strong.structuremanagement._2013_05.VariantManagement.GetBOMVariantRuleInput;
import com.teamcenter.soa.client.Connection;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.BOMWindow;
import com.teamcenter.soa.client.model.strong.Item;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.soa.client.model.strong.PSBOMView;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.soa.client.model.strong.VariantRule;
import com.teamcenter.soa.common.ObjectPropertyPolicy;
import com.teamcenter.soa.exceptions.NotLoadedException;

public class VariantManagement
{
	private DataManagementService dmService;
	private StructureManagementService structureService;
	private VariantManagementService variantService;
	
	private Item item;
	private VariantRule variantRule;
	
	private static final String itemUID = "QGUhUOOl4aTB3C";

	
	public VariantManagement(User user, Connection connection)
	{
		dmService = DataManagementService.getService(connection);
		structureService = StructureManagementService.getService(connection);
		variantService = VariantManagementService.getService(connection);
		
		setObjectPolicy();
	}
	
	public boolean loadObjects()
	{
		ServiceData sData = dmService.loadObjects(new String[]{ itemUID });

		if(!ServiceDataError(sData))
		{
			boolean success = false;
			
			for(int i = 0; i < sData.sizeOfPlainObjects(); i++)
			{
				ModelObject mObj = sData.getPlainObject(i);
	
				if(mObj instanceof Item)
				{
					item = (Item) mObj;
					success = true;
				}
				else
					return false;
			}
			
			return success;
		}

		return false;
	}
	
	
	public void listBOMVariantRulesFromBOMWindow()
	{
		try
		{
			ModelObject[] itemRevs = item.get_revision_list();
			ModelObject[] bomViews = item.get_bom_view_tags();

			for(ModelObject bomView : bomViews)
			{
				for(ModelObject itemRev : itemRevs)
				{
					StructureManagementService.CreateBOMWindowsInfo bomWinInfo = new StructureManagementService.CreateBOMWindowsInfo();

					bomWinInfo.item = item;
					bomWinInfo.itemRev = (ItemRevision) itemRev;
					bomWinInfo.bomView = (PSBOMView) bomView;

					StructureManagementService.CreateBOMWindowsResponse bomResp = structureService.createBOMWindows(new StructureManagementService.CreateBOMWindowsInfo[]{ bomWinInfo });

					if(!ServiceDataError(bomResp.serviceData))
					{
						for(CreateBOMWindowsOutput creatOutput : bomResp.output)
						{
							GetBOMVariantRuleInput ruleInput = new GetBOMVariantRuleInput();
							
							ruleInput.clientId = "ID:" + ruleInput.hashCode();
							ruleInput.svrActionMode = 0; // to get variant rule from window. 
							ruleInput.window = creatOutput.bomWindow;
							
							BOMVariantRulesResponse resp = variantService.getBOMVariantRules(new GetBOMVariantRuleInput[]{ ruleInput });
							
							if(!ServiceDataError(resp.serviceData))
							{
								for(BOMVariantRuleOutput ruleOutput : resp.variantRuleData)
								{
									for(BOMVariantRuleContents ruleContent : ruleOutput.rules)
									{
										for(BOMVariantOptionValueEntry option : ruleContent.bomVariantOptionValueEntry)
										{
											System.out.println("Name: " + option.optionName + ", " + option.optionDesc);
										}
									}
								}
							}
							
							structureService.closeBOMWindows(new BOMWindow[]{ creatOutput.bomWindow });
						}
					}
				}
			}
		}
		catch(ServiceException e)
		{
			e.printStackTrace();
		}
		catch(NotLoadedException e)
		{
			e.printStackTrace();
		}
		catch(java.lang.ClassCastException e)
		{
			e.printStackTrace();
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

		policy.addType("Item", new String[]{ "bom_view_tags", "revision_list" });
		policy.addType("BOMLine", new String[]{ "bl_line_name" });
		policy.addType("BOMWindow", new String[]{ "is_packed_by_default" });

		session.setObjectPropertyPolicy(policy);
	}
}
