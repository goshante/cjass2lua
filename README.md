# cjass2lua
## Useful converter from cJass to Lua for Warcraft III scripts

## Description
Warcraft III Reforged brought us lua scripting, but still a lot of map makers for WC3 are using cJass. Just because C-sytle syntax rules. Unfortunately cJass extension is unsupported for Reforged and who knows how it will be. Lua is more efficient, useful and simple for game development. But a lot of projects are still on cJass and no way to convert them to Lua. Editor without cJass extension does not perceive this code. Re-writing code manualy to Lua could take a lot of time, nobody wants to waste time on it. This tool comes to solve this problem. cjass2lua tool can easily convert cJass to readable Lua code, it will help you to upgrade your map by modern map development patterns.

### Features
- Analysing and syntax checking cJass code
- Producing .lua file with converted code of your cJass code
- Advanced logging of application
- Opportunity to implement output for any other script language
- 100% readable code formatting
- Saving comments, comment positions and new lines, function and variable names, spaces in expressions
- Config file with application's settings
- Minimalistic and simple GUI interface

### Unsupported features
- Analyzing classes/structures and converting them to Lua meta table implementation
- JASS/vJASS Syntax
- Pre-processor directives
- Macroses. Only constants, function and variable shorters. Macroses are translated into global variables.

### Latest release
- [v1.00](https://github.com/fullmetal-a/cjass2lua/releases/tag/v1.00)

### Manual
#### GUI
1. Enter destination path of cJass text file (or folder with cJass files)
2. Enter destination path of output Lua file (or folder) to save Lua file(s)
3. Click Translate
#### Console
- First argument is input file or folder (necessary)
- Second argument is output file or folder (optional)

### Config options description
- ConvertRawCodes - Can be TRUE or FALSE. If TRUE - converts raw codes (such as 'A0D3' or 'I0AX') in corresponding integers. If FALSE wraps raw code into FourCC() Lua call.
- IgnoreComments - Can be TRUE or FALSE. If TRUE - all comments will be ignored.
- LogLevel - Value between 0 and 4. Minimal allowed log message type. 0 - Debug, 1 - Info, 2 - Warning, 3 - Critical, 4 - Fatal.
- OutputLanguage - Only "Lua" is available. An output language to translate. 
- OutputNewLineType - Can be "CR", "LF" or "CRLF". Type of new line in file.
- StrictMode - Can be TRUE or FALSE. If TRUE - you must close every operation with ';'. Causes errors when ';' is missing. If FALSE - new line is an end for operation (but ';' is also accepted).

### Troubleshooting
Create an issue and pin your log file with debug log level (0) if you shure that problem is in application.
