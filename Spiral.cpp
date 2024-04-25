#include "../Public/Spiral.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"



ASpiral::ASpiral(const FObjectInitializer& ObjectInitializer)
{
	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	BaseTrack = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("MainSpline"));
	BaseTrack->SetupAttachment(RootComponent);
#if WITH_EDITOR
	BaseTrack->EditorUnselectedSplineSegmentColor = FColor::Yellow;
#endif
}

void ASpiral::OnConstruction(const FTransform& Transform)
{
	if (bDrawDebug)
	{
		DrawDebugSpiral();
	}
}

FVector ASpiral::GetPositionAtDistance(float Distance)
{
	if (Distance < 0) Distance = 0;
	float SplineLen = BaseTrack->GetSplineLength();

	//find the normalized distance along the base spline so that we can do LERP
	float alpha = Distance / SplineLen;

	//get position and orientation at the given position of the spline so that we can place our spiral position
	FTransform baseXForm = BaseTrack->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	FRotator baseRot = baseXForm.GetRotation().Rotator();

	//find our desired angle for the proportionate distance on the spline
	float Theta = (ThetaStart + SpiralCount * 360.0f) * alpha;
	
	FVector rightVec = UKismetMathLibrary::GetRightVector(baseRot);
	FVector fwdVec = UKismetMathLibrary::GetForwardVector(baseRot);

	float radius = 0;
	if (RadiusCurve != nullptr)
	{
		float min, max;
		RadiusCurve->FloatCurve.GetTimeRange(min, max);

		//convert the spline distance range into a proportionate range on the float curve
		float lerpDist = alpha * max;
		radius = RadiusCurve->GetFloatValue(lerpDist);
		radius *= CurveScalar;
	}
	else
	{
		radius = UKismetMathLibrary::Lerp(DefaultRadius, DefaultEndRadius, alpha);
	}

	//rotate our right vector around the forward vector axis of rotation by the theta value amount
	rightVec = UKismetMathLibrary::RotateAngleAxis(rightVec, Theta, fwdVec);

	FVector ret = baseXForm.GetLocation() + rightVec * radius;
	return ret;
}

FVector ASpiral::GetPositionAtAngle(float theta)
{
	float maxAngle = ThetaStart + SpiralCount * 360.0f;
	theta = UKismetMathLibrary::Clamp(theta, 0, maxAngle);
	float alpha = theta / maxAngle;
	float SplineLen = BaseTrack->GetSplineLength();
	float SplineDist = SplineLen * alpha;

	//get position and orientation at the given position of the spline so that we can place our spiral position
	FTransform baseXForm = BaseTrack->GetTransformAtDistanceAlongSpline(SplineDist, ESplineCoordinateSpace::World);
	FRotator baseRot = baseXForm.GetRotation().Rotator();
	FVector rightVec = UKismetMathLibrary::GetRightVector(baseRot);
	FVector fwdVec = UKismetMathLibrary::GetForwardVector(baseRot);

	float radius = 0;
	if (RadiusCurve != nullptr)
	{
		float min, max;
		RadiusCurve->FloatCurve.GetTimeRange(min, max);

		//convert the spline distance range into a proportionate range on the float curve
		float lerpDist = alpha * max;
		radius = RadiusCurve->GetFloatValue(lerpDist);
		radius *= CurveScalar;
	}
	else
	{
		radius = UKismetMathLibrary::Lerp(DefaultRadius, DefaultEndRadius, alpha);
	}

	//rotate our right vector around the forward vector axis of rotation by the theta value amount
	rightVec = UKismetMathLibrary::RotateAngleAxis(rightVec, theta, fwdVec);

	FVector ret = baseXForm.GetLocation() + rightVec * radius;
	return ret;

}

void ASpiral::DrawDebugSpiral()
{
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		UKismetSystemLibrary::FlushPersistentDebugLines(World);

		//UKismetSystemLibrary::DrawDebugPoint(World, xform.GetLocation(), 10, FLinearColor::Red, 15);

		/*float r = 0;
		float rMax = ThetaStart + SpiralCount * 360.0f;
		while (r < rMax)
		{
			UKismetSystemLibrary::DrawDebugPoint(World, GetPositionAtAngle(r), 10, FLinearColor::Red, 15);
			r += 1;
		}*/

		float length = BaseTrack->GetSplineLength();
		const float stepSize = 0.1f;
		float d = 0;
		FVector lastPos = GetPositionAtDistance(0);
		while (d < length)
		{
			d += stepSize;
			FVector curPos = GetPositionAtDistance(d);
			UKismetSystemLibrary::DrawDebugLine(World, lastPos, curPos, FLinearColor::Red, 30);
			lastPos = curPos;
		}

	}
}