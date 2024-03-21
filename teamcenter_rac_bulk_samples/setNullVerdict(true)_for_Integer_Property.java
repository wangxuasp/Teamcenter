/* 
 * p is instance of TCProperty
 */
if( value.length() > 0 )
                        {
                            Integer i = Integer.valueOf( value );
                            p.setIntValueData( i.intValue() );

                            if( Debug.isOn( AbstractPropertyBean.DEBUG ) )
                            {
                                Debug.println( "TextfieldPropertyBean:save propName=" //$NON-NLS-1$
                                        + property + ",set int = " + value ); //$NON-NLS-1$
                            }
                        }
                        else
                        {
                            if( isSetPropValueNULL( p.getTCComponent().getSession() ) )
                            {
                                p.setPropValueNull( true );
                            }
                            else
                            {
                                p.setNullVerdict( true );
                            }


isSetPropValueNULL() calls 

TCPreferenceService prefService = session.getPreferenceService();
return prefService.getLogicalValue( "FORM_text_field_shown_NULL" ); //$NON-NLS-1$
