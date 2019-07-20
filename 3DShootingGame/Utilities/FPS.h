#pragma once
#include <Common/StepTimer.h>

class FPS
{
public:
	void update()
	{
		// Keep track of the time lapse and frame count
		mFrames++;
		mFrames2++;

		mInstantFPSUpdated = false;
		mFPSUpdated = false;

		// Instant frame rate
		float Delta = mTimer2.peekElapsedSeconds();
		if (Delta > 0.01f)
		{
			mInstantFPS = mFrames2 / Delta;
			mInstantFPSUpdated = true;
			mTimer2.getElapsedSeconds();
			mFrames2 = 0;
		}

		// Update the frame rate once per second
		Delta = mTimer.peekElapsedSeconds();
		if (Delta > 1.0f)
		{
			mFPS = mFrames / Delta;
			mFPSUpdated = true;
			mTimer.getElapsedSeconds();
			mFrames = 0;
		}
	}
	float getFPS()		const { return mFPS; }
	float getInstantFPS()	const { return mInstantFPS; }
	bool hasFPSChanged()			const { return mFPSUpdated; }
	bool hasInstantFPSChanged()	const { return mInstantFPSUpdated; }

	FPS(DX::StepTimer& stepTimer)
		: mTimer(stepTimer)
		, mTimer2(stepTimer)
		, mFPS(0.f)
		, mInstantFPS(0.f)
		, mFPSUpdated(false)
		, mInstantFPSUpdated(false)
		, mFrames(0)
		, mFrames2(0)
	{
	}

private:
	struct Time
	{
		using Second = float;
		DX::StepTimer& stepTimer;
		float lastTime;

		Time(DX::StepTimer& stepTimer)
			: stepTimer(stepTimer)
			, lastTime(0)
		{
			getElapsedSeconds();
		}

		Second getElapsedSeconds()
		{
			float time = lastTime;
			lastTime = float(stepTimer.GetTotalSeconds());
			return lastTime - time;
		}

		Second peekElapsedSeconds()
		{
			return float(stepTimer.GetTotalSeconds()) - lastTime;
		}
	};

	Time			mTimer;
	Time			mTimer2;
	float			mFPS;
	float			mInstantFPS;
	bool			mFPSUpdated;
	bool			mInstantFPSUpdated;
	int				mFrames;
	int				mFrames2;
};
