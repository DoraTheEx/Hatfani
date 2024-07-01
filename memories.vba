Sub CreateKerberosPresentation()
    ' Create a new PowerPoint presentation
    Dim pptApp As Object
    Dim pptPres As Object
    Dim pptSlide As Object
    Dim pptShape As Object
    
    ' Initialize PowerPoint application
    Set pptApp = CreateObject("PowerPoint.Application")
    pptApp.Visible = True
    
    ' Add a new presentation
    Set pptPres = pptApp.Presentations.Add
    
    ' Slide 1: Title Slide
    Set pptSlide = pptPres.Slides.Add(1, 1) ' 1 = ppLayoutTitle
    pptSlide.FollowMasterBackground = msoFalse
    pptSlide.Background.Fill.ForeColor.RGB = RGB(0, 102, 204) ' Dark blue background
    pptSlide.Shapes.Title.TextFrame.TextRange.Text = "Kerberos Authentication Mechanism"
    pptSlide.Shapes.Title.TextFrame.TextRange.Font.Color.RGB = RGB(255, 255, 255) ' White text
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Text = "An Overview of the Process"
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Font.Color.RGB = RGB(255, 255, 255) ' White text
    pptSlide.SlideShowTransition.EntryEffect = ppEffectFade ' Add fade transition

    ' Slide 2: Introduction to Kerberos
    Set pptSlide = pptPres.Slides.Add(2, 2) ' 2 = ppLayoutText
    pptSlide.FollowMasterBackground = msoFalse
    pptSlide.Background.Fill.ForeColor.RGB = RGB(102, 204, 255) ' Light blue background
    pptSlide.Shapes.Title.TextFrame.TextRange.Text = "Introduction to Kerberos"
    pptSlide.Shapes.Title.TextFrame.TextRange.Font.Color.RGB = RGB(0, 51, 102) ' Dark blue text
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Text = "• Kerberos is a network authentication protocol." & vbCrLf & _
                                                              "• It is designed to provide strong authentication for client-server applications." & vbCrLf & _
                                                              "• Uses secret-key cryptography and a trusted third party (Key Distribution Center - KDC)."
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Font.Color.RGB = RGB(0, 51, 102) ' Dark blue text
    pptSlide.SlideShowTransition.EntryEffect = ppEffectFade ' Add fade transition
    
    ' Slide 3: Components of Kerberos
    Set pptSlide = pptPres.Slides.Add(3, 2) ' 2 = ppLayoutText
    pptSlide.FollowMasterBackground = msoFalse
    pptSlide.Background.Fill.ForeColor.RGB = RGB(153, 204, 255) ' Lighter blue background
    pptSlide.Shapes.Title.TextFrame.TextRange.Text = "Components of Kerberos"
    pptSlide.Shapes.Title.TextFrame.TextRange.Font.Color.RGB = RGB(0, 51, 102) ' Dark blue text
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Text = "• Key Distribution Center (KDC): Central authority in Kerberos, comprising two main parts:" & vbCrLf & _
                                                              "  - Authentication Server (AS)" & vbCrLf & _
                                                              "  - Ticket Granting Server (TGS)" & vbCrLf & _
                                                              "• Principal: Any entity (user or service) that participates in the Kerberos protocol." & vbCrLf & _
                                                              "• Tickets: Credentials used to prove the identity of a principal."
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Font.Color.RGB = RGB(0, 51, 102) ' Dark blue text
    pptSlide.SlideShowTransition.EntryEffect = ppEffectFade ' Add fade transition
    
    ' Slide 4: The Authentication Process
    Set pptSlide = pptPres.Slides.Add(4, 2) ' 2 = ppLayoutText
    pptSlide.FollowMasterBackground = msoFalse
    pptSlide.Background.Fill.ForeColor.RGB = RGB(204, 229, 255) ' Even lighter blue background
    pptSlide.Shapes.Title.TextFrame.TextRange.Text = "The Authentication Process"
    pptSlide.Shapes.Title.TextFrame.TextRange.Font.Color.RGB = RGB(0, 51, 102) ' Dark blue text
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Text = "1. Initial Authentication:" & vbCrLf & _
                                                              "   - The client sends a request to the AS for a Ticket Granting Ticket (TGT)." & vbCrLf & _
                                                              "   - The AS verifies the client's credentials and responds with an encrypted TGT." & vbCrLf & _
                                                              "2. Service Request:" & vbCrLf & _
                                                              "   - The client sends the TGT to the TGS to request access to a specific service." & vbCrLf & _
                                                              "   - The TGS issues a service ticket." & vbCrLf & _
                                                              "3. Accessing the Service:" & vbCrLf & _
                                                              "   - The client presents the service ticket to the service server." & vbCrLf & _
                                                              "   - The server verifies the ticket and grants access."
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Font.Color.RGB = RGB(0, 51, 102) ' Dark blue text
    pptSlide.SlideShowTransition.EntryEffect = ppEffectFade ' Add fade transition
    
    ' Slide 5: Detailed Steps of Kerberos Authentication
    Set pptSlide = pptPres.Slides.Add(5, 2) ' 2 = ppLayoutText
    pptSlide.FollowMasterBackground = msoFalse
    pptSlide.Background.Fill.ForeColor.RGB = RGB(230, 242, 255) ' Lightest blue background
    pptSlide.Shapes.Title.TextFrame.TextRange.Text = "Detailed Steps of Kerberos Authentication"
    pptSlide.Shapes.Title.TextFrame.TextRange.Font.Color.RGB = RGB(0, 51, 102) ' Dark blue text
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Text = "1. The client sends a request to the AS for a TGT." & vbCrLf & _
                                                              "2. The AS responds with an encrypted TGT if credentials are verified." & vbCrLf & _
                                                              "3. The client sends the TGT to the TGS to request a service ticket." & vbCrLf & _
                                                              "4. The TGS responds with the service ticket." & vbCrLf & _
                                                              "5. The client presents the service ticket to the service server." & vbCrLf & _
                                                              "6. The service server verifies the ticket and grants access."
    pptSlide.Shapes.Placeholders(2).TextFrame.TextRange.Font.Color.RGB = RGB(0, 51, 102) ' Dark blue text
    pptSlide.SlideShowTransition.EntryEffect = ppEffectFade ' Add fade transition
    
    ' Release the PowerPoint objects
    Set pptSlide = Nothing
    Set pptPres = Nothing
    Set pptApp = Nothing
End Sub
