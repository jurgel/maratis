-----------------------------------------------------------------------------------
-- Maratis
-- ShadTest.lua
-----------------------------------------------------------------------------------

-- get objects
TPot = getObject("TPot")
Camera = getObject("Camera")
Pivot = getObject("Pivot")

-- scene update
function onSceneUpdate()

	rotate(TPot, {0, 0, 1}, 2, "global")
	rotate(Pivot, {0, 0, 1}, 4, "global")
	translate(Camera, {-0.1, 0, 0});
	
end