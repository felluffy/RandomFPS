// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "VoiceHttpSTTComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRecognizedSpeechSignature, UVoiceHttpSTTComponent*, STTComponent, float, AccuracyScore, FString, SentenceRetreived);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_AT2_API UVoiceHttpSTTComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVoiceHttpSTTComponent();
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRecognizedSpeechSignature OnRecognizedSpeech;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
	void SerializeRecivedAudio(class USoundWave* SoundWave);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	class FHttpModule* Http;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Request Properties")
	FString URL;
	UPROPERTY(EditDefaultsOnly, Category = "Request Properties")
	FString ContentType;
	UPROPERTY(EditDefaultsOnly, Category = "Request Properties")
	FString VerbType;
	UPROPERTY(EditDefaultsOnly, Category = "Request Properties")
	FString FileLocation;
	UPROPERTY(EditDefaultsOnly, Category = "Request Properties")
	FString	OAuthToken;

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	//void SendAudioRequest(const FString& Url, const FString& RequestContent);
	UFUNCTION(BlueprintCallable)
	void SendAudioRequest();
};
