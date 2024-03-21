package com.gtac.rac.stylesheet;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import java.util.HashMap;
import java.util.Map;

import com.teamcenter.rac.kernel.TCComponentForm;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCProperty;
import com.teamcenter.rac.kernel.TCSession;

import com.teamcenter.rac.stylesheet.AbstractRendering;
import com.teamcenter.rac.stylesheet.PropertyTextField;
import com.teamcenter.rac.util.MessageBox;
import com.teamcenter.rac.util.PropertyLayout;
import com.teamcenter.rac.util.VerticalLayout;

public class MyAbstractRendering extends AbstractRendering 
{
	private JTextField designer_jtextfield;
    private JTextField checker_jtextfield;
    
    private TCProperty designer_tcproperty;
    private TCProperty checker_tcproperty;
    
    private TCComponentForm form;

    public MyAbstractRendering( TCComponentForm c)throws Exception
    {
        super ( c );
        form = c;
        loadRendering(); 
    }

    public void loadRendering() throws TCException 
    {
        initializeUI();
        
        designer_tcproperty = form.getFormTCProperty("designer");
        checker_tcproperty = form.getFormTCProperty("checker");
       
        designer_jtextfield.setText(designer_tcproperty.getStringValue());
        checker_jtextfield.setText(checker_tcproperty.getStringValue());     
    }

	
    public void saveRendering() 
    {
        try
        {
            designer_tcproperty.setStringValueData(designer_jtextfield.getText() );
            checker_tcproperty.setStringValueData(checker_jtextfield.getText() );

            TCProperty[] ps = new TCProperty[2];
            ps[0] = designer_tcproperty;
            ps[1] = checker_tcproperty;

            form.setTCProperties(ps);
        }
        catch ( TCException ex )
        {
            MessageBox.post(ex.getMessage(), null, MessageBox.ERROR);
        }
    }

    @Override	
    public boolean isRenderingModified()
    {
        if( designer_tcproperty != null && !designer_jtextfield.getText().equals( designer_tcproperty.getStringValue() ) )
        {
            return true;
        }

        if( checker_tcproperty != null && !checker_jtextfield.getText().equals( checker_tcproperty.getStringValue() ) )
        {
            return true;
        } 
        else return false;
    }
    
    @Override 
    public Map getRenderingModified ()
    {
        Map modifiedRendering = new HashMap<String, Object> ();
        if( designer_tcproperty != null && !designer_jtextfield.getText().equals( designer_tcproperty.getStringValue() ) )
        {
            designer_tcproperty.setStringValueData(designer_jtextfield.getText() );
            modifiedRendering.put( "designer", designer_tcproperty );
        }

        if( checker_tcproperty != null && !checker_jtextfield.getText().equals( checker_tcproperty.getStringValue() ) )
        {
            checker_tcproperty.setStringValueData( checker_jtextfield.getText() );
            modifiedRendering.put( "checker", checker_tcproperty );
        }
        return modifiedRendering;
    }

    private void initializeUI()
    {
        setLayout ( new VerticalLayout() );
        JPanel mainPanel = new JPanel( new PropertyLayout());
        mainPanel.setOpaque(false);
        mainPanel.setEnabled(false);
        
        // Create all the text fields
        designer_jtextfield = new JTextField(15);
        checker_jtextfield = new JTextField(15);
        
        //Add components to Panel
        mainPanel.add("1.1.right.center",new JLabel("Designer's Name"));
        mainPanel.add("1.2.left.center", designer_jtextfield);

        mainPanel.add("2.1.right.center",new JLabel("Checker's Name"));
        mainPanel.add("2.2.left.center", checker_jtextfield);
        add("unbound.bind", mainPanel);
    }
}
