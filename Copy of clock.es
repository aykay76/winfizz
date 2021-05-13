K3D:init
{
	Textures:Add("earth", "c:\alan.kelly\oak.jpg");
	Materials:Add("earthmat", Colour(1, 1, 1), Colour(1, 1, 1), Colour(1, 1, 1), Colour(0.1, 0.1, 0.1), 128, "earth");
	
	Textures:Add("granite", "c:\alan.kelly\granite.jpg");
	Materials:Add("granitemat",  Colour(1, 1, 1), Colour(1, 1, 1), Colour(1, 1, 1), Colour(0, 0, 0), 254, "granite");

	i = 1;
	Cameras:Add("cam" + i, Vector(0, 0, 60), 30);

	Lights:Add("spot", Colour(1, 1, 1), Colour(1, 1, 1), Colour(0, 0, 0), Vector(-10, 10, 10, 1), Vector(0.7, -0.7, 1), 30, 91);

	Templates:Torus("notch", 0.5, 1.0, 10, 20);
	
	angle = 0;
	while (angle < 360)
	{
		Entities:Add("notch" + i, "notch", Vector(0, 10, 0):rotate(angle, Vector(0, 0, -1)));
		i++;
		angle += 30;
	}

	i = 1;
	while (i <= 12)
	{
		Entities["notch" + i]:material = "earthmat";
		Entities["notch" + i]:rotate(90, Vector(1, 0, 0));
		i++;
	}

	Templates:Cylinder("hand", 0.2, 10.0, 10, 10);
	Templates:Cylinder("shorthand", 0.2, 8, 10, 10);
	Entities:Add("secondhand", "hand", Vector(0.0, 5.0, 0.0));
	Entities:Add("minutehand", "hand", Vector(0.0, 5.0, 0.0));
	Entities:Add("hourhand", "shorthand", Vector(0.0, 4, 0.0));
	
	Entities["minutehand"]:material = "granitemat";
	Entities["secondhand"]:material = "granitemat";
	Entities["hourhand"]:material = "granitemat";

	Templates:Sphere("centre", 1.0, 3);
	Entities:Add("centrepiece", "centre", Vector(0, 0, 0));
	Entities["centrepiece"]:material = "earthmat";

	Entities["minutehand"]:rotate(31 * 6, Vector(0, 0, -1), 1);
	Entities["hourhand"]:rotate(3 * 30, Vector(0, 0, -1), 1);

	Timers:Add("tenthsecond", 100, 0);
	Timers:Add("minute", 60000, 0);
	Timers:Add("hour", 3600000, 0);
}

tenthsecond:tick
{
	Entities["secondhand"]:rotate(0.6, Vector(0, 0, -1), 1);
	
	i = 1;
	while (i <= 12)
	{
		Entities["notch" + i]:rotate(0.6, Vector(1, 0, 0), 0);
		i++;
	}
}

minute:tick
{
	Entities["minutehand"]:rotate(6, Vector(0, 0, -1), 1);
}

hour:tick
{
	Entities["hourhand"]:rotate(6, Vector(0, 0, -1), 1);
}

K3D:frameStart
{
}

K3D:leftButtonDown
{
}