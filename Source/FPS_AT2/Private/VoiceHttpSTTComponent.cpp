// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceHttpSTTComponent.h"
#include "Runtime/JsonUtilities/Public/JsonUtilities.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "Sound/SoundWave.h"
#include "Interfaces/IHttpRequest.h"

// Sets default values for this component's properties
UVoiceHttpSTTComponent::UVoiceHttpSTTComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Http = &FHttpModule::Get();
	URL = TEXT("https://centralindia.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1?language=en-US&format=detailed");
	FileLocation = TEXT("C:\\Programming Tools\\Unreal Projects\\ABC.wav");
	OAuthToken = TEXT("fcafc499185f474388c6c46e13a44d71");
	VerbType = "POST";
}


void UVoiceHttpSTTComponent::SerializeRecivedAudio_Implementation(USoundWave* SoundWave)
{
	/*FArchive fr;
	if (SoundWave != NULL)
	{
		SoundWave->Serialize(fr);
		auto text = SoundWave->GetPathName();
		UE_LOG(LogTemp, Error, TEXT("PATH FOLDER %s"), *text);
	}
	else
	{
		auto Sd = LoadObject<USoundWave>(NULL, *FileLocation, NULL, LOAD_None, NULL);
		SoundWave->Serialize(fr);
		UE_LOG(LogTemp, Error, TEXT("Not Saved"));
	}*/
}

// Called when the game starts
void UVoiceHttpSTTComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UVoiceHttpSTTComponent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hey we received the following response!"));
		UE_LOG(LogTemp, Warning, TEXT("bbb %s"), *Response->GetContentAsString());
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			auto it = JsonObject->GetArrayField("NBest");
			if (it.begin() != it.end())
			{
				auto JsonMid = it[0]->AsObject();

				float Accuracy = JsonMid->GetNumberField("Confidence");
				FString ToReturnSentence = JsonMid->GetStringField("Display");
				OnRecognizedSpeech.Broadcast(this, Accuracy, ToReturnSentence); 
			}
		}
		
	}
}

void UVoiceHttpSTTComponent::SendAudioRequest(FString Path)
{
	//FileLocation = Path;
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UVoiceHttpSTTComponent::OnResponseReceived);
	Request->SetURL(URL);
	Request->SetVerb(VerbType);
	Request->SetHeader("Content-Type", TEXT("audio/wav"));
	Request->AppendToHeader("Content-Type", TEXT("codecs=audio/pcm"));
	Request->AppendToHeader("Content-Type", TEXT("samplerate=16000"));
	Request->SetHeader(TEXT("Ocp-Apim-Subscription-Key"), OAuthToken);
	Request->SetHeader("Accept", "application/json; text/xml");
	Request->SetHeader("Expect", "100-continue");
	Request->SetHeader("Host", "centralindia.speech.microsoft.com");
	bool resB = Request->SetContentAsStreamedFile(FileLocation);
	UE_LOG(LogTemp, Error, TEXT("File found %d, file loc - %s"), resB, *FileLocation);

	Request->ProcessRequest();
}

