K3D:init
{
	i = 0;
	
	Cameras:Add("cam1", Vector(0.0, 0.0, 0.0), 30.0);

	Lights:Add("spot", Colour(1, 1, 1, 1), Colour(1, 1, 1), Colour(0, 1, 0), Vector(0, 1, 0, 0), Vector(0, 1, 1), 0.91, 50);

	Templates:Sphere("ball", 10.0, 4);
	Entities:Add("ball1", "ball", Vector(0.0, 0.0, -50.0));
	
	Templates:Add("plane", "spitfire.3ds");
	Entities:Add("spitfire", "plane", Vector(0.0, 0.0, 500.0));
}

K3D:frameStart
{
}

K3D:leftButtonDown
{
	if (i == 0)
	{
		Entities["spitfire"]:rotatei(Entities["spitfire"]:position, Vector(0, 0, 1), 360, 1);
	}
	else
	{
		Entities["spitfire"]:rotatei(Entities["spitfire"]:position, Vector(0, 1, 1), 360, 1);
	}
	
	i++;
	if (i > 1) i = 0;
}