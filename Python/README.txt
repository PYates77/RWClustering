INSTRUCTIONS FOR RUNNING RWGUI.py
>python RWGUI.py <font_parameter>
 <font_parameter> can either be one of the three options below:
    (1) --native (which assumes you are not using SSH and X11 forwarding)
    (2) --x11 (which assumes you are using SSH and X11 forwarding)
    (3) <value> (any integer value if the above two presets aren't working for you)
    
This GUI script is also run as part of the RWCExecute.csh script if the --gui flag is passed to it.

For more information, ./RWCExecute.csh --help or go RWClustering/docs/ to read the RWGUI manual