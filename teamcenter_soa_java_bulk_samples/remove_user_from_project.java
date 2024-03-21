/*
*
    The SOA Java HelloTeamcenter project found in 
    soa_client.zip was used to test this method.
	
	This sample calls listMembers method from 
		Sample SOA Java method : print Project team members (Document ID: tc_api594)
*
*/

import java.util.List;
import java.util.ArrayList;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core.ProjectLevelSecurityService;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.GroupMember;
import com.teamcenter.soa.client.model.strong.TC_Project;
import com.teamcenter.services.strong.core._2012_09.ProjectLevelSecurity.ProjectClientId;
import com.teamcenter.services.strong.core._2012_09.ProjectLevelSecurity.ProjectTeamsResponse;
import com.teamcenter.services.strong.core._2012_09.ProjectLevelSecurity.TeamMemberInfo;
import com.teamcenter.services.strong.core._2012_09.ProjectLevelSecurity.ProjectInformation;
import com.teamcenter.services.strong.core._2012_09.ProjectLevelSecurity.ModifyProjectsInfo;
import com.teamcenter.services.strong.core._2012_09.ProjectLevelSecurity.ProjectOpsResponse; 
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.soa.client.model.ServiceData;



    private final int MEMBER_TYPE_REGULAR = 0;
    private final int MEMBER_TYPE_PRIVILEGED = 1;
    private final int MEMBER_TYPE_TEAM_ADMIN = 2;
    
    public void removeFromProject(String userId, String groupName, String roleName, 
		String projectId, boolean removePrivStatus, boolean removeAdminStatus)
    {
    	DataManagementService dmService = 
    		DataManagementService.getService(AppXSession.getConnection());
    	
        ProjectLevelSecurityService prjService = 
			ProjectLevelSecurityService.getService(AppXSession.getConnection());
        
    	Query query = new Query();
   
    	User user = query.findUser(userId);
    	String userUID = user.getUid();
   	
    	GroupMember  groupMember = query.findGroupMember(groupName, roleName, userId); 
    	String groupMemberUID = groupMember.getUid();
    	
		TC_Project project = query.findProject(projectId);
        listMembers(project);
		
		ProjectClientId projectclientid = new ProjectClientId();
		projectclientid.clientId= projectclientid.toString();
		projectclientid.tcProject = project;
	
		ProjectTeamsResponse ptRsp = 
			prjService.getProjectTeams(new ProjectClientId[] {projectclientid });
		int regLen = ptRsp.projectTeams[0].regularMembers.length;
		int privLen = ptRsp.projectTeams[0].privMembers.length;
		int adminLen = ptRsp.projectTeams[0].projectTeamAdmins.length;
	
		int nUsersGroupMembers = 0;
		String[] attributes = {"the_user"};
        dmService.getProperties(ptRsp.projectTeams[0].regularMembers, attributes);
		
		List<ModelObject> membersList = null;
		membersList = new ArrayList<ModelObject>();
		for(int j = 0; j < regLen; j++)
		{
			GroupMember aMember = (GroupMember) ptRsp.projectTeams[0].regularMembers[j];
			try
			{
				String groupMemberUserUID = aMember.get_the_user().getUid();
				if(groupMemberUserUID == userUID)
					nUsersGroupMembers = nUsersGroupMembers + 1;
			}
			catch (Exception e) 
			{
				System.out.println(e.getMessage());
			}
			
			if (!ptRsp.projectTeams[0].regularMembers[j].getUid().equals(groupMemberUID))
			{
				membersList.add(ptRsp.projectTeams[0].regularMembers[j]);
			}
		}
		
		ModelObject [] members = null;
		members = membersList.toArray( new ModelObject[membersList.size() ] );
		ptRsp.projectTeams[0].regularMembers = members;

		membersList = new ArrayList<ModelObject>();
		for(int j = 0; j < privLen; j++)
		{
			if (!ptRsp.projectTeams[0].privMembers[j].getUid().equals(userUID))
			{
				membersList.add(ptRsp.projectTeams[0].privMembers[j]);
			}
			else if (nUsersGroupMembers > 1 && removePrivStatus == false)
			{	    
				membersList.add(ptRsp.projectTeams[0].privMembers[j]);
			}		
		}	
		members = membersList.toArray( new ModelObject[membersList.size() ] );
		ptRsp.projectTeams[0].privMembers = members;

		membersList = new ArrayList<ModelObject>();
		for(int j = 0; j < adminLen; j++)
		{
			if (!ptRsp.projectTeams[0].projectTeamAdmins[j].getUid().equals(userUID))
			{
				membersList.add(ptRsp.projectTeams[0].projectTeamAdmins[j]);
			}
			else if (nUsersGroupMembers > 1 && removeAdminStatus == false)
			{	    
				membersList.add(ptRsp.projectTeams[0].projectTeamAdmins[j]);
			}
		}
		members = membersList.toArray( new ModelObject[membersList.size() ] );
		ptRsp.projectTeams[0].projectTeamAdmins = members;		
		
        // Things have changed so get new lengths
		regLen = ptRsp.projectTeams[0].regularMembers.length;
		privLen = ptRsp.projectTeams[0].privMembers.length;
		adminLen = ptRsp.projectTeams[0].projectTeamAdmins.length;
				
		int totalLength = regLen + privLen + adminLen;
					 		
		TeamMemberInfo memberInfo[] = new TeamMemberInfo[totalLength];
		int index = 0;
		for(int j = 0; j < regLen; j++)
		{			
	     	memberInfo[index] = new TeamMemberInfo();
	     	memberInfo[index].teamMember = ptRsp.projectTeams[0].regularMembers[j];
	     	memberInfo[index].teamMemberType = MEMBER_TYPE_REGULAR;
	     	index++;
		}
		
		for(int j = 0; j < privLen; j++)
		{		
	     	memberInfo[index] = new TeamMemberInfo();
	     	memberInfo[index].teamMember = ptRsp.projectTeams[0].privMembers[j];
	     	memberInfo[index].teamMemberType = MEMBER_TYPE_PRIVILEGED;
	     	index++;
		}
     	
		for(int j = 0; j < adminLen; j++)
		{			
	     	memberInfo[index] = new TeamMemberInfo();
	     	memberInfo[index].teamMember = ptRsp.projectTeams[0].projectTeamAdmins[j];
	     	memberInfo[index].teamMemberType = MEMBER_TYPE_TEAM_ADMIN;
	     	index++;
		}
          
		ProjectInformation projInfo = new ProjectInformation();
		projInfo.clientId = projInfo.toString();
        projInfo.projectDescription = "Modifed";
        projInfo.projectId = projectId;
        projInfo.projectName = projectId;
        projInfo.active = true;
        projInfo.teamMembers = memberInfo;

		ModifyProjectsInfo modifyInput = new ModifyProjectsInfo();
		
		modifyInput.clientId = modifyInput.toString();
		modifyInput.sourceProject = ptRsp.projectTeams[0].project;
        modifyInput.projectInfo = projInfo;

        ProjectOpsResponse resp = 
			prjService.modifyProjects(new ModifyProjectsInfo[] {modifyInput});
       
        if(!ServiceDataError(resp.serviceData))
        {
        	System.out.println("Project succesfully Modified !");
        }
        else
        {
        	System.out.println("Project Modification failed ...");
        }
        listMembers(project);		
    }