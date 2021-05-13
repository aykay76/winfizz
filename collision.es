K3D:init
{
	Textures:Add("earth", "c:\alan.kelly\check.jpg");
	Materials:Add("earthmat", Colour(1, 1, 1), Colour(1, 1, 1), Colour(1, 1, 1), Colour(0.1, 0.1, 0.1), 128, "earth");
	
	Cameras:Add("cam1", Vector(0, 0, 0), 30);
	Lights:Add("spot", Colour(1, 1, 1), Colour(1, 1, 1), Colour(0, 0, 0), Vector(-10, 10, 10, 1), Vector(0.7, -0.7, 1), 30, 91);

	Templates:Sphere("ball", 2, 2, 5);
	Entities:Add("ball1", "ball", Vector(0, 0, -50));

	Entities["ball1"]:forces = Vector(0, 0, 0);

	Timers:Add("timer", 36, 0);
}

timer:tick
{
	Entities["ball1"]:forces = Vector(0, 0, 0);
}

K3D:frameStart
{
}

K3D:leftButtonDown
{
}