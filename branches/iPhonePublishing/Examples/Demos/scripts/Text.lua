-----------------------------------------------------------------------------------
-- Maratis
-- Jules script test
-----------------------------------------------------------------------------------

-- get objects
Text0 = getObject("Text0")
Text1 = getObject("Text1")

t = 0

-- scene update
function onSceneUpdate()

	text = "Exemple de texte controlé en script :\n"
	text = text .. getText(Text1, text) .. "\n"
	text = text .. "t = " .. t

	setText(Text0, text)
	t = t + 1

end
