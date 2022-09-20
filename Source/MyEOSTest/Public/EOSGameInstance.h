// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSGameInstance.generated.h"


//DECLARE_LOG_CATEGORY_EXTERN(LogEOSTest, Log, All);
//
//void LogString(const FString& str);
//
//#define PRINTF(format, ...) { FString str = FString::Printf(format, __VA_ARGS__); LogString(str); }


/**
 * 
 */
UCLASS()
class MYEOSTEST_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEOSGameInstance();

	virtual void Init();

	UFUNCTION(BlueprintCallable)
	void Login();
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(BlueprintCallable)
	void CreateSession();
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void LoginDS();
	void OnLoginDSComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(BlueprintCallable)
	void CreateDSServerSession();
	void OnCreateDSServerSessionComplete(FName SessionName, bool bWasSuccessful);
	UFUNCTION(BlueprintCallable)
		void DestroyDSServerSession();
	void OnDestroyDSServerSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void DestroySession();
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void FindSessions();
	void OnFindSessionsComplete(bool bWasSuccessful);
	//TSharedRef<FOnlineSessionSearch> SearchSettings = MakeShareable(new FOnlineSessionSearch());
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);

	UFUNCTION(BlueprintCallable)
	void GetAllFriends();
	void OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	UFUNCTION(BlueprintCallable)
	void ShowInviteUI();

	UFUNCTION(BlueprintCallable)
	void ShowFriendsUI();

	UFUNCTION(BlueprintCallable)
	void ShowLoginUI();

	UPROPERTY(EditAnywhere)
	FString MyTestAccount = FString("EOSTestPlayer");

	UPROPERTY(EditAnywhere)
	bool isLAN = false;
	UPROPERTY(EditAnywhere)
	bool isSearchLAN = false;
	UPROPERTY(EditAnywhere)
	bool IsDedicated = false;

	UPROPERTY(EditAnywhere)
		bool IsIPNot0 = true;

	UFUNCTION(BlueprintCallable)
		void SetIsIPNot0(bool isstr);

	UFUNCTION(BlueprintCallable)
		void SetMyisLAN(bool isstr);
	
	UFUNCTION(BlueprintCallable)
		void SetMyisSearchLAN(bool isstr);

	UFUNCTION(BlueprintCallable)
		void SetMyIsDedicated(bool isstr);

	UFUNCTION(BlueprintCallable)
	void SetMyTestAccount(FString str);

protected:
	class IOnlineSubsystem* OnlineSubsystem;
	FName InstanceName;
	FName OnlineSubsystemName;
	FText OnlineServiceName;
	FString AccountTickName;
	bool bIsLoggedIn = false;

protected:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;

	virtual void OnCreateSteamSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSteamSessionComplete(bool Succeeded);
	virtual void OnJoinSteamSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
		void CreateSteamServer();

	UFUNCTION(BlueprintCallable)
		void JoinSteamServer();

public:
	UFUNCTION(BlueprintCallable)
		void ConnectIP(FString ip);

	UFUNCTION(BlueprintCallable)
		void ConnectSessionIP(FString ip);

	void OnConnectSessionIPComplete(FName SessionName, bool Succeeded);

	FString MyIP;

	UFUNCTION(BlueprintCallable)
		void TravelToGameMap();
};
