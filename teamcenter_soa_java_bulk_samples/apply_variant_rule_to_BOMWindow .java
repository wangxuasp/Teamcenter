Use this VariantManagement class as follows and update the itemUID with one of your Item objects: 

   VariantManagement vm = new VariantManagement(user, AppXSession.getConnection());
       
   if(vm.loadObjects())
        vm.setVariantRule();



The java SOA HelloTeamcenter example was used to test this code. 


/************************************************************************************/
package com.teamcenter.hello;

import java.util.Map;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.schemas.soa._2006_03.exceptions.ServiceException;
import com.teamcenter.services.strong.cad.StructureManagementService;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement.CreateBOMWindowsOutput;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement.GetVariantRulesResponse;
import com.teamcenter.services.strong.cad._2019_06.StructureManagement.CreateWindowsInfo3;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core.SessionService;
import com.teamcenter.services.strong.structuremanagement.VariantManagementService;
import com.teamcenter.services.strong.structuremanagement._2013_05.VariantManagement.BOMVariantOptionValueEntry;
import com.teamcenter.services.strong.structuremanagement._2019_06.VariantManagement.BOMVariantRuleContents2;
import com.teamcenter.services.strong.structuremanagement._2019_06.VariantManagement.BOMVariantRuleOutput2;
import com.teamcenter.services.strong.structuremanagement._2019_06.VariantManagement.BOMVariantRulesResponse2;
import com.teamcenter.services.strong.structuremanagement._2013_05.VariantManagement.GetBOMVariantRuleInput;
import com.teamcenter.services.strong.structuremanagement._2019_06.VariantManagement.ApplyBOMVariantRulesResponse2;
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
 
  private static final String itemUID = "<ADD YOUR OWN UID>"; // "QGUhUOOl4aTB3C";
 
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
 
  public void setVariantRule()
  {
     BOMWindow bomWindow = openBOMWindow(item);
     VariantRule variantRule = getVariantRule(item);
    
     if(bomWindow != null && variantRule != null)
     {
        try
        {
           BOMVariantRuleContents2 ruleContent = new BOMVariantRuleContents2();
          
           ruleContent.bomVariantOptionValueEntry = new BOMVariantOptionValueEntry[0];
           ruleContent.isConstraintsEvaluated = true;
           ruleContent.isSVRModified = false;
           ruleContent.svr = variantRule;
           ruleContent.variantRule = null; // Used with BOMVariantRule runtime object
          
           ApplyBOMVariantRulesResponse2 appBomVarRuleRes = variantService.applyBOMVariantRules2(bomWindow, new BOMVariantRuleContents2[]{ ruleContent });
          
           if(!ServiceDataError(appBomVarRuleRes.serviceData))
           {
              structureService.saveBOMWindows(new BOMWindow[]{ bomWindow });
           }
          
           structureService.closeBOMWindows(new BOMWindow[]{ bomWindow });
        }
        catch (ServiceException e)
        {
           e.printStackTrace();
        }
     }
  }
 
  protected BOMWindow openBOMWindow(Item item)
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

              StructureManagementService.CreateBOMWindowsResponse bomResp = structureService.createOrReConfigureBOMWindows(new CreateWindowsInfo3[]{ bomWinInfo });

              if(!ServiceDataError(bomResp.serviceData))
              {
                 for(CreateBOMWindowsOutput creatOutput : bomResp.output)
                 {
                    return creatOutput.bomWindow;
                 }
              }
           }
        }
     }
     catch(NotLoadedException e)
     {
        e.printStackTrace();
     }
     catch(java.lang.ClassCastException e)
     {
        e.printStackTrace();
     }
    
     return null;
  }
 
 
  protected VariantRule getVariantRule(final Item item)
  {
     try
     {
        ModelObject[] itemRevs = item.get_revision_list();
       
        if(itemRevs.length > 0)
        {
           ItemRevision itemRev = (ItemRevision)itemRevs[0];
          
           GetVariantRulesResponse ruleResp = structureService.getVariantRules(new ItemRevision[]{ itemRev });
          
            if(!ServiceDataError(ruleResp.serviceData))
              {
               Map<ItemRevision, VariantRule[]> map = ruleResp.inputItemRevToVarRules;
               
               if(map.containsKey(itemRev))
               {
                  VariantRule[] rules = map.get(itemRev);
                  
                  if(rules.length > 0)
                     return rules[0];
               }
                  
              }
        }
     }
     catch(ServiceException | NotLoadedException e)
     {
        e.printStackTrace();
     }
    
     return null;
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