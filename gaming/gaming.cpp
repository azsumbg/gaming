#include "pch.h"
#include "gaming.h"

// RandIt *****************************

dll::RandIt::RandIt()
{
	sq = new std::seed_seq{ rd(), rd(), rd(), rd(), rd(), rd() };
	twister = new std::mt19937(*sq);
}
dll::RandIt::~RandIt()
{
	if (sq)delete sq;
	if (twister)delete twister;
}

int dll::RandIt::operator() (int min, int max)
{
	std::uniform_int_distribution distr{ min,max };

	return distr(*twister);
}

///////////////////////////////////////

// PROTON *****************************

dll::PROTON::PROTON(float sx, float sy, float s_width, float s_height) :width{ s_width }, height{ s_height }
{
	start.x = sx;
	start.y = sy;
	
	end.x = sx + width;
	end.y = sy + height;

	center.x = start.x + width / 2;
	center.y = start.y + height / 2;
}

void dll::PROTON::SetEdges()
{
	end.x = start.x + width;
	end.y = start.y + height;

	center.x = start.x + width / 2;
	center.y = start.y + height / 2;
}
void dll::PROTON::NewDims(float new_width, float new_height)
{
	width = new_width;
	height = new_height;

	end.x = start.x + width;
	end.y = start.y + height;

	center.x = start.x + width / 2;
	center.y = start.y + height / 2;
}

float dll::PROTON::GetWidth() const
{
	return width;
}
float dll::PROTON::GetHeight() const
{
	return height;
}

// BASE *************************************

dll::BASE::BASE(uint8_t _what, float _sx, float _sy) :PROTON(_sx, _sy)
{
	type = _what;

	switch (type)
	{
	case evil1:
		NewDims(80.0f, 48.0f);
		max_frames = 11;
		frame_delay = 7;
		break;

	case evil2:
		NewDims(80.0f, 86.0f);
		max_frames = 3;
		frame_delay = 23;
		break;

	case evil3:
		NewDims(85.0f, 96.0f);
		max_frames = 3;
		frame_delay = 23;
		break;

	case civilian:
		NewDims(40.0f, 60.0f);
		max_frames = 45;
		frame_delay = 2;
		break;

	case hero:
		NewDims(80.0f, 40.0f);
		max_frames = 2;
		frame_delay = 35;
		break;

	case supply:
		NewDims(40.0f, 35.0f);
		max_frames = 26;
		frame_delay = 3;
		break;

	case bullet:
		NewDims(10.0f, 25.0f);
		break;

	}
}
void dll::BASE::SetPathInfo(float _where_x, float _where_y)
{
	move_sx = start.x;
	move_ex = _where_x;
	move_sy = start.y;
	move_ey = _where_y;

	hor_line = false;
	vert_line = false;

	if ((move_sx == move_ex) || (move_ex == end.x) || (move_ex < move_sx && move_ex >= move_sx - width))
	{
		vert_line = true;
		return;
	}
	if ((move_sy == move_ey) || (move_ey == end.y) || (move_ey < move_sy && move_ey >= move_sy - height))
	{
		hor_line = true;
		return;
	}

	slope = (move_ey - move_sy) / (move_ex - move_sx);
	intercept = move_sy - slope * move_sx;
}

uint8_t dll::BASE::GetType()const
{
	return type;
}
void dll::BASE::SetType(uint8_t to_what)
{
	type = to_what;

	switch (type)
	{
	case evil1:
		NewDims(80.0f, 48.0f);
		max_frames = 11;
		frame_delay = 7;
		break;

	case evil2:
		NewDims(80.0f, 86.0f);
		max_frames = 3;
		frame_delay = 23;
		break;

	case evil3:
		NewDims(85.0f, 96.0f);
		max_frames = 3;
		frame_delay = 23;
		break;

	case civilian:
		NewDims(40.0f, 60.0f);
		max_frames = 45;
		frame_delay = 2;
		break;

	case hero:
		NewDims(80.0f, 40.0f);
		max_frames = 2;
		frame_delay = 35;
		break;

	case supply:
		NewDims(40.0f, 35.0f);
		max_frames = 26;
		frame_delay = 3;
		break;
	}
}

int dll::BASE::GetFrame()
{
	--frame_delay;
	if (frame_delay <= 0)
	{
		switch (type)
		{
		case evil1:
			frame_delay = 7;
			break;

		case evil2:
			frame_delay = 23;
			break;

		case evil3:
			frame_delay = 23;
			break;

		case civilian:
			frame_delay = 2;
			break;

		case hero:
			frame_delay = 35;
			break;

		case supply:
			frame_delay = 3;
			break;
		}

		++frame;
		if (frame > max_frames)frame = 0;
	}

	return frame;
}

// OBJECTS ***********************************

dll::OBJECTS::OBJECTS(uint8_t what, float init_x, float init_y, float bull_dest_x, float bull_dest_y)
	:BASE(what, init_x, init_y)
{
	switch (type)
	{
	case civilian:
		speed = 0.5f;
		strenght = 2;
		if (start.x <= scr_width / 2)dir = dirs::left;
		else dir = dirs::right;
		break;

	case bullet:
		speed = 5.0f;
		strenght = 20;
		SetPathInfo(bull_dest_x, bull_dest_y);
		break;

	case supply:
		speed = 1.0f;
		strenght = 0;
		break;
	}
}
	
bool dll::OBJECTS::Move(float gear)
{
	float my_speed = speed + gear / 5.0f;

	if (type == civilian)
	{
		switch (dir)
		{
		case dirs::left:
			if (start.x - my_speed > 0)
			{
				start.x -= my_speed;
				SetEdges();
				return true;
			}
			else
			{
				dir = dirs::right;
				return false;
			}
			break;

		case dirs::right:
			if (end.x + my_speed <scr_width)
			{
				start.x += my_speed;
				SetEdges();
				return true;
			}
			else
			{
				dir = dirs::left;
				return false;
			}
			break;
		}
	}
	else if (type == bullet)
	{
		if (start.x >= scr_width || end.x <= 0 || start.y >= ground || end.y <= sky)return false;

		if (hor_line)
		{
			if (move_ex == move_sx)return false;
			else if (move_ex < move_sx)
			{
				start.x -= my_speed;
				SetEdges();
				return true;
			}
			else
			{
				start.x += my_speed;
				SetEdges();
				return true;
			}
		}
		if (vert_line)
		{
			if (move_ey == move_sy)return false;
			else if (move_ey < move_sy)
			{
				start.y -= my_speed;
				SetEdges();
				return true;
			}
			else
			{
				start.y += my_speed;
				SetEdges();
				return true;
			}
		}

		if (move_ex == move_sx)return false;
		else if (move_ex < move_sx)
		{
			start.x -= my_speed;
			start.y = start.x * slope + intercept;
			SetEdges();
			return true;
		}
		else
		{
			start.x += my_speed;
			start.y = start.x * slope + intercept;
			SetEdges();
			return true;
		}
	}
	else if (type == supply)
	{
		start.y += my_speed;
		SetEdges();
		if (end.y >= ground)return false;
		return true;
	}

	return false;
}
void dll::OBJECTS::Release()
{
	delete this;
}

// CREATURES *********************************

dll::CREATURES::CREATURES(uint8_t what, float init_x, float init_y) :BASE(what, init_x, init_y)
{
	switch (what)
	{
	case evil1:
		lifes = 20;
		strenght = 5;
		break;

	case evil2:
		lifes = 25;
		strenght = 8;
		break;

	case evil3:
		lifes = 35;
		strenght = 10;
		break;

	case hero:
		lifes = 40;
		strenght = 10;
		break;
	}
}

bool AIShoot(cont::CONTAINER<FPOINT> civs, FPOINT Hero_pos, FPOINT& where_to_shoot);

bool dll::CREATURES::Move(float gear)
{

}
void dll::CREATURES::Release()
{
	delete this;
}

// FACTORIES *********************************

GAMING_API dll::OBJECTS* dll::ObjectFactory(uint8_t which_object, float deploy_x, float deploy_y,
	float bullet_dest_x, float bullet_dest_y)
{
	Asset ret = nullptr;
	
	switch (which_object)
	{
	case bullet:
		ret = new OBJECTS(bullet, deploy_x, deploy_y, bullet_dest_x, bullet_dest_y);
		break;

	case supply:
		ret = new OBJECTS(supply, deploy_x, deploy_y);
		break;

	case civilian:
		ret = new OBJECTS(civilian, deploy_x, deploy_y);
		break;
	}

	return ret;
}

GAMING_API dll::CREATURES* dll:: CreatureFactory(uint8_t which_object, float deploy_x, float deploy_y)
{
	dll::Creature ret{ nullptr };

	switch (which_object)
	{
	case evil1:
		ret = new dll::CREATURES(evil1, deploy_x, deploy_y);
		break;

	case evil2:
		ret = new dll::CREATURES(evil2, deploy_x, deploy_y);
		break;

	case evil3:
		ret = new dll::CREATURES(evil3, deploy_x, deploy_y);
		break;

	case hero:
		ret = new dll::CREATURES(hero, deploy_x, deploy_y);
		break;
	}

	return ret;
}