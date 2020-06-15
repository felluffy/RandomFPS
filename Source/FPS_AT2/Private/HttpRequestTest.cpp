#include "HttpRequestTest.h"
//#include "Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "Runtime/JsonUtilities/Public/JsonUtilities.h"
//? 

AHttpRequestTest::AHttpRequestTest()
{
	Http = &FHttpModule::Get();
	PrimaryActorTick.bCanEverTick = false;
	FRequest_Login LoginCredentials;
	LoginCredentials.email = TEXT("asdf@asdf.com");
	LoginCredentials.password = TEXT("asdfasdf");
	//Login(LoginCredentials);
}

void AHttpRequestTest::BeginPlay()
{
	Super::BeginPlay();
	SendAudioRequest(URLToSendTo, ContentType);
}

void AHttpRequestTest::SendAudioRequest(const FString& Url, const FString& RequestContent)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	//Binding a function that will fire when we receive a response from our request
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpRequestTest::OnResponseReceived);

	Request->SetURL(Url);
	Request->SetVerb(VerbType);
	//Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");

	Request->SetHeader("Content-Type", TEXT("audio/wav")); 
	Request->AppendToHeader("Content-Type", TEXT("codecs=audio/pcm"));
	Request->AppendToHeader("Content-Type", TEXT("samplerate=16000"));
	bool resB = Request->SetContentAsStreamedFile(FileLocation);

	Request->SetHeader(TEXT("Ocp-Apim-Subscription-Key"), OAuthToken);
	Request->SetHeader("Accept", "application/json; text/xml");
	Request->SetHeader("Expect", "100-continue");
	Request->SetHeader("Host", "centralindia.speech.microsoft.com");
	/*UE_LOG(LogTemp, Error, TEXT("File found %d"), resB);
	auto re = Request->GetAllHeaders();
	for (auto &it : re)
	{
		UE_LOG(LogTemp, Error, TEXT("Header: %s"), *it);
	}
*/

	//Send the request
	Request->ProcessRequest();
}

