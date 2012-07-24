-----------------------------------------------------------------------------------
-- Maratis
-- CartoonTPot.lua
-----------------------------------------------------------------------------------

-- get objects
LightPivot = getObject("LightPivot")


-- scene update
function onSceneUpdate()

	rotate(LightPivot, {0, 0, 1}, 1, "global")
	
end