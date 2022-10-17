# clipboardIntercept
## About
Built to extract contents of windows clipboard and save in datastructures. This is a proof of concept, this code could be expanded on to build functionality as seen in @Pieces.app
## Usage
The current code will take the contents of the clipboard, save contents in selfmade structure, delete contents from clipboard, and relaod the daved contents to the clipboard. 
Minimal diagnostics are shown for this, it should be noted that some formatts cannot be saved, causing the contents of the clipboard to sometimes be slightly different. 
This loss happens when handles are stored in the clipboard, some standard windows formatts are handles, making this frequent.
## References
Author - Zach Kreutzjans
