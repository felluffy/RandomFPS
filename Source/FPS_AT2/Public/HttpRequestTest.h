// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "GameFramework/Actor.h"
#include "Runtime/JsonUtilities/Public/JsonUtilities.h"
#include "Runtime/Online/HTTP/Public/Http.h"
//#include "HttpRetrySystem.h"
//#include "HttpModule.h"
//#include "HttpRetrySystem.h"
//#include "HttpRequestAdapter.h"
//#include "HttpManager.h"
#include "HttpRequestTest.generated.h"

/**
 * 
 */

USTRUCT()
struct FRequest_Login {
	GENERATED_BODY()
		UPROPERTY() FString email;
	UPROPERTY() FString password;

	FRequest_Login() {}
};

USTRUCT()
struct FResponse_Login {
	GENERATED_BODY()
		UPROPERTY() int id;
	UPROPERTY() FString name;
	UPROPERTY() FString hash;

	FResponse_Login() {}
};


UCLASS()
class FPS_AT2_API AHttpRequestTest : public AActor
{
	GENERATED_BODY()
public:
	AHttpRequestTest();
	virtual void BeginPlay() override;


	//void Login(FRequest_Login LoginCredentials);
	//void LoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
private:
	class FHttpModule* Http;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString URLToSendTo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ContentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString VerbType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString FileLocation;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString OAuthToken;
private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hey we received the following response!"));
			UE_LOG(LogTemp, Warning, TEXT("bbb %s"), *Response->GetContentAsString());
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Didn't get anything"));
	}

	void SendAudioRequest(const FString& Url, const FString& RequestContent);

	void SendHttpRequest(const FString& Url, const FString& RequestContent)
	{

		//Creating a request using UE4's Http Module
		TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

		//Binding a function that will fire when we receive a response from our request
		Request->OnProcessRequestComplete().BindUObject(this, &AHttpRequestTest::OnResponseReceived);

		//This is the url on which to process the request
		Request->SetURL(Url);
		//We're sending data so set the Http method to POST
		Request->SetVerb(VerbType);

		//Tell the host that we're an unreal agent
		Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");

		//In case you're sending json you can also make use of headers using the following command. 
			//Just make sure to convert the Json to string and use the SetContentAsString.
		Request->SetHeader("Content-Type", TEXT("application/json"));
		//Use the following command in case you want to send a string value to the server
		//Request->SetContentAsString("Hello kind server.");

		//Send the request
		Request->ProcessRequest();
	}
};
