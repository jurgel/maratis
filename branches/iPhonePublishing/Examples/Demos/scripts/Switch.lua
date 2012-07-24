-----------------------------------------------------------------------------------
-- Maratis
-- Jules script test
-----------------------------------------------------------------------------------

-- get objects
Obj1 = getObject("Obj1")
Obj2 = getObject("Obj2")

activate(Obj1)
deactivate(Obj2)

switch = 1

-- scene update
function onSceneUpdate()

	-- switch
	if onKeyDown("SPACE") then
		if switch == 1 then
			switch = 2
			setRotation(Obj2, {0, 0, 180})
			activate(Obj2)
			deactivate(Obj1)
		else
			switch = 1
			setRotation(Obj1, {0, 0, 180})
			activate(Obj1)
			deactivate(Obj2)
		end
	end
	
	-- rotate
	if switch == 1 then
		rotation = getRotation(Obj1)
		if rotation[3] > 0 then
			rotate(Obj1, {0, 0, 1}, -10, "global")
		end
	else
		rotation = getRotation(Obj2)
		if rotation[3] > 0 then
			rotate(Obj2, {0, 0, 1}, -10, "global")
		end
	end
	
end