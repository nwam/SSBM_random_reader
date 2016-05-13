#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.

F1::
delay:=30

Loop 100 {
	Send {x Down}
	Sleep %delay%
	Send {x Up}
	Send {z Down}
	Sleep %delay%
	Send {z Up}
	Sleep %delay%
}
return