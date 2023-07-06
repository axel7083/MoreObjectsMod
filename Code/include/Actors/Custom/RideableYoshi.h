#pragma once

struct RideableYoshi : Actor
{
	struct State
	{
		using StateFunc = void(RideableYoshi::*)();
		StateFunc init;
		StateFunc main;
	};
	
	enum Animations
	{
		WAIT,
		RIDE,
		RUN,
		
		NUM_ANIMS,
	};
	
	enum EndRideState
	{
		ER_END_RIDE_NO_CHANGE_CHAR = -1,
		ER_END_RIDE_CHANGE_CHAR = 1,
		ER_END_RIDE_CHANGE_CHAR_NO_RUN = 2,
		ER_DO_NOT_END_RIDE = 0,
	};
	
	static State ST_WAIT;
	static State ST_RIDE;
	static State ST_RUN;
	
	ModelAnim modelAnim; // when riding, this is the rider
	Model* headModel;    // pointer because Yoshi's head is a ModelAnim, while the other heads aren't
	MovingCylinderClsn cylClsn;
	WithMeshClsn wmClsn;
	ShadowModel shadow;
	Matrix4x3 shadowMat;
	Vector3 origPos;
	Player* rider;
	State* state;
	u8 riderChar;
	u8 cooldown;
	bool riding;
	u16 runTimer;
	
	static SpawnInfo spawnData;
	static SharedFilePtr ridingAnimFile;
	
	RideableYoshi();
	virtual int InitResources() override;
	virtual int CleanupResources() override;
	virtual int Behavior() override;
	virtual int Render() override;
	virtual ~RideableYoshi() override;
	
	void UpdateModelTransform();
	void DropShadow();
	void CheckCylClsn();
	void ChangeState(State* newState);
	void StartRide(int charID);
	void EndRide(bool changeChar);
	
	void St_Wait_Init();
	void St_Wait_Main();
	void St_Ride_Init();
	void St_Ride_Main();
	void St_Run_Init();
	void St_Run_Main();
	
	static s32 ShouldEndRide(Player& player); // -1: End ride, do not change character. 1: End ride, change character. 0: Do not end ride
	static void ChangeCharacter(Player& player, u32 character);
};