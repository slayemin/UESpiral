#pragma once

#include "Components/SplineComponent.h"
#include "Curves/CurveFloat.h"

#include "Spiral.generated.h"

/*A spiral is just a bunch of circles stacked on top of each other, but as we trace the outlines of each
circle, we increase the height. We need to specify the number of spirals, start radius, end radius, and orientation.*/
UCLASS()
class VRTECH_API ASpiral : public AActor
{
	GENERATED_BODY()

public:

	ASpiral(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* SceneRoot;

	//This is the center line of the spiral. It doesn't have to be linear.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USplineComponent* BaseTrack;

	//The lathe curve describes how the radius of the spiral changes as a function of distance along the spiral track.
	//normally, this would be a constant or linear value, but you can do some crazy wobbly stuff here too.
	//If this is specified, it will override the default radius and end radius
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spiral")
		UCurveFloat* RadiusCurve = nullptr;

	//The curve scalar is a multiplier to scale the radius of the curve
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spiral")
		float CurveScalar = 1.0f;

	//This is the default radius of the spiral.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spiral")
	float DefaultRadius = 100;

	//This is the default end radius of the spiral.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spiral")
	float DefaultEndRadius = 100;

	//The amount of radians to offset the spiral starting position. This determines where the winding begins.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spiral")
		float ThetaStart = 0;

	//This indicates the number of spirals between the start and end.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spiral")
		float SpiralCount = 1;

	//the winding direction is clockwise
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spiral")
		bool bRotateClockwise = true;

	//draws the debug spiral to the screen
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
		bool bDrawDebug = false;

	//For the given distance along the base track, we get a transform for the corresponding spiral position
	UFUNCTION(BlueprintCallable, Category = "Spirals")
		FTransform GetTransformAtDistance(float Distance);


protected:
	//helper function to get a spiral position at a given distance
	FVector GetPositionAtDistance(float Distance);

	//Given an angle between 0 and spiralCount * 360, this gives you the 3D position on the spiral
	FVector GetPositionAtAngle(float theta);
	

private:

	void DrawDebugSpiral();

};