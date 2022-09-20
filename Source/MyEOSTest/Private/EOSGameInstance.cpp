// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"


#include "Interfaces/OnlineIdentityInterface.h"


#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// 关闭优化
#pragma optimize("", off) 

//constexpr FName SessionName = FName("Test Session");
const static FName TestSessionName("Test Session kid");

//DEFINE_LOG_CATEGORY(LogEOSTest);
//void LogString(const FString& str)
//{
//	UE_LOG(LogEOSTest, Warning, TEXT("%s"), *str);
//
//	GEngine->AddOnScreenDebugMessage((uint64)-1, 5.0f, FColor::Cyan, str);
//
//}


UEOSGameInstance::UEOSGameInstance()
{
	UE_LOG(LogTemp, Warning, TEXT("EOSGameInstance Successful initialization"));
}

void UEOSGameInstance::Init()
{
	Super::Init();

	OnlineSubsystem = IOnlineSubsystem::Get();
	//OnlineSubsystem = IOnlineSubsystem::Get("NULL");
	
	InstanceName = OnlineSubsystem->GetInstanceName();
	OnlineSubsystemName = OnlineSubsystem->GetSubsystemName();
	OnlineServiceName = OnlineSubsystem->GetOnlineServiceName();

	//class FOnlineSubsystemEOS* OnlineSubsystemEOS = static_cast<FOnlineSubsystemEOS*>(OnlineSubsystem);
	
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "Steam")
	{
		InstanceName = OnlineSubsystem->GetInstanceName();
		OnlineSubsystemName = OnlineSubsystem->GetSubsystemName();
		OnlineServiceName = OnlineSubsystem->GetOnlineServiceName();

		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSteamSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSteamSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSteamSessionComplete);
		}
	}
	// Login();
}

void UEOSGameInstance::Login()
{
	if (OnlineSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubsystem:InstanceName is %s!!!  OnlineSubsystemName is %s!!!  OnlineServiceName is %s!!!"),
			*InstanceName.ToString(), *OnlineSubsystemName.ToString(), *OnlineServiceName.ToString());

		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credientials;
			
			if (true) {
				Credientials.Id = FString("localhost:9999");
				Credientials.Token = MyTestAccount; // like Player1
				Credientials.Type = FString("developer");
			}
			else
			{
				if (true) {
					// EOS_DevAuthTool.exe
					Credientials.Id = FString("localhost:9999");
					Credientials.Token = FString("EOSTestPlayer"); // like Player1
					Credientials.Type = FString("developer");
					//Credientials.Type = TEXT("developer");
				}
				else
				{
					Credientials.Id = FString("");
					Credientials.Token = FString(""); // like Player1
					Credientials.Type = FString("accountportal");
				}
			}

			Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);
			
			// login 有问题，没有产生回调
			Identity->Login(0, Credientials);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot Login: Cannot GetIdentityInterface"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Login: Not OnlineSubsystem"));
	}
}


void UEOSGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{

	//PRINTF(TEXT("OnLoginComplete: Success %d"), bWasSuccessful, *Error);
	UE_LOG(LogTemp, Warning, TEXT("OnLoginComplete: Success %d"), bWasSuccessful);

	if (!bWasSuccessful)
	{
		//PRINTF(TEXT("OnLoginComplete: failure error %s"), *Error);

		UE_LOG(LogTemp, Warning, TEXT("OnLoginComplete: failure error %s"), *Error);
	}

	bIsLoggedIn = bWasSuccessful;

	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FUniqueNetIdPtr LocalUserIdPtr = Identity->GetUniquePlayerId(0);

			ELoginStatus::Type tmpType = Identity->GetLoginStatus(0);

			AccountTickName = Identity->GetPlayerNickname(0);
			FString AuthToken = Identity->GetAuthToken(0);
			FPlatformUserId tmpPlatformUserId = Identity->GetPlatformUserIdFromLocalUserNum(0);
			UE_LOG(LogTemp, Warning, TEXT("OnLoginComplete: AccountTickName is %s"), *AccountTickName);


			Identity->ClearOnLoginCompleteDelegates(0, this);
		}
	}
}

void UEOSGameInstance::CreateSession()
{
	//if (bIsLoggedIn || IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") {
	if (true) {
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				UE_LOG(LogTemp, Warning, TEXT("CreateSession Start!"));

				FOnlineSessionSettings SessionSettings;
				SessionSettings.bIsDedicated = IsDedicated;
				SessionSettings.bShouldAdvertise = true;

				//SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
				SessionSettings.bIsLANMatch = isLAN;

				SessionSettings.NumPublicConnections = 5;
				SessionSettings.bAllowJoinInProgress = true;
				SessionSettings.bAllowJoinViaPresence = true;
				SessionSettings.bAllowInvites = true;
				SessionSettings.bUsesPresence = true;

				SessionSettings.bUseLobbiesIfAvailable = true; // lobby相关
				// 设置成可以portal在Lobby中找到
				SessionSettings.Set(SEARCH_KEYWORDS, FString("MyEOSTestLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

				SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionComplete);

				SessionPtr->CreateSession(0, TestSessionName, SessionSettings);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Create Session: Not Logged In"));
	}
	
}

void UEOSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful) 
{
	UE_LOG(LogTemp,Warning, TEXT("OnCreateSessionComplete: %s Success %d"), *SessionName.ToString(), bWasSuccessful);
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{


			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);

			UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete"));
			// 跳转
			//GetWorld()->ServerTravel(FString("MyWorld?listen"), false);
			GetWorld()->ServerTravel(FString("ThirdPersonMap?listen"), false);
			//GetWorld()->GetFirstPlayerController()->ClientTravel(FString("127.0.0.1"), TRAVEL_Absolute);
		}
	}
}

void UEOSGameInstance::LoginDS()
{
	if (OnlineSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubsystem:InstanceName is %s!!!  OnlineSubsystemName is %s!!!  OnlineServiceName is %s!!!"),
			*InstanceName.ToString(), *OnlineSubsystemName.ToString(), *OnlineServiceName.ToString());

		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credientials;

			if (true) {
				Credientials.Id = FString("127.0.0.1:6666");
				Credientials.Token = FString("EOSTestPlayer"); // like Player1
				Credientials.Type = FString("developer");
			}
			else
			{
				if (true) {
					// EOS_DevAuthTool.exe
					Credientials.Id = FString("127.0.0.1:6666");
					Credientials.Token = FString("EOSTestPlayer"); // like Player1
					Credientials.Type = FString("developer");
					//Credientials.Type = TEXT("developer");
				}
				else
				{
					Credientials.Id = FString("");
					Credientials.Token = FString(""); // like Player1
					Credientials.Type = FString("accountportal");
				}
			}

			Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);

			// login 有问题，没有产生回调
			Identity->Login(0, Credientials);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot Login: Cannot GetIdentityInterface"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Login: Not OnlineSubsystem"));
	}
}

void UEOSGameInstance::OnLoginDSComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	//PRINTF(TEXT("OnLoginComplete: Success %d"), bWasSuccessful, *Error);
	UE_LOG(LogTemp, Warning, TEXT("OnLoginDSComplete: Success %d"), bWasSuccessful);

	if (!bWasSuccessful)
	{
		//PRINTF(TEXT("OnLoginComplete: failure error %s"), *Error);

		UE_LOG(LogTemp, Warning, TEXT("OnLoginDSComplete: failure error %s"), *Error);
	}

	bIsLoggedIn = bWasSuccessful;

	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FUniqueNetIdPtr LocalUserIdPtr = Identity->GetUniquePlayerId(0);

			ELoginStatus::Type tmpType = Identity->GetLoginStatus(0);

			AccountTickName = Identity->GetPlayerNickname(0);
			FString AuthToken = Identity->GetAuthToken(0);
			FPlatformUserId tmpPlatformUserId = Identity->GetPlatformUserIdFromLocalUserNum(0);
			UE_LOG(LogTemp, Warning, TEXT("OnLoginComplete: AccountTickName is %s"), *AccountTickName);


			Identity->ClearOnLoginCompleteDelegates(0, this);
		}

		CreateDSServerSession();
	}
}
void UEOSGameInstance::CreateDSServerSession()
{
	//if (bIsLoggedIn || IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") {
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			UE_LOG(LogTemp, Warning, TEXT("CreateDSServerSession Start!"));

			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsDedicated = true;
			SessionSettings.bShouldAdvertise = true;

			//SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
			SessionSettings.bIsLANMatch = false;

			SessionSettings.NumPublicConnections = 6;
			SessionSettings.bAllowJoinInProgress = false;
			SessionSettings.bAllowJoinViaPresence = false;
			SessionSettings.bAllowInvites = false;
			SessionSettings.bUsesPresence = false;

			SessionSettings.bUseLobbiesIfAvailable = false; // lobby相关
			// 设置成可以portal在Lobby中找到
			//SessionSettings.Set(SEARCH_KEYWORDS, FString("MyEOSDSLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

			SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateDSServerSessionComplete);

			SessionPtr->CreateSession(0, FName("Test Session"), SessionSettings);
		}
	}
	
}

void UEOSGameInstance::OnCreateDSServerSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateDSServerSessionComplete: %s Success %d"), *SessionName.ToString(), bWasSuccessful);
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
			// 跳转
			//GetWorld()->ServerTravel(FString("MyWorld?listen"), false);
			//GetWorld()->ServerTravel(FString("ThirdPersonMap?listen"), false);
			UE_LOG(LogTemp, Warning, TEXT("OnCreateDSServerSessionComplete, end to create ds server!"));
		}
	}
}

void UEOSGameInstance::DestroyDSServerSession()
{
	//if (bIsLoggedIn || IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroyDSServerSessionComplete);
				SessionPtr->DestroySession(FName("Test Session"));
			}
		}
	}
}

void UEOSGameInstance::OnDestroyDSServerSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
		}
	}
}



void UEOSGameInstance::DestroySession()
{
	//if (bIsLoggedIn || IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionComplete);
				SessionPtr->DestroySession(TestSessionName);
			}
		}
	}
}

void UEOSGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
		}
	}
}

void UEOSGameInstance::FindSessions()
{
	if (bIsLoggedIn || IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr Ptr = OnlineSubsystem->GetSessionInterface())
			{
				SearchSettings = MakeShareable(new FOnlineSessionSearch());

				SearchSettings->MaxSearchResults = 5000;
				SearchSettings->bIsLanQuery = isSearchLAN;

				SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("MyEOSTestLobby"), EOnlineComparisonOp::Equals);
				SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
				
				

				UE_LOG(LogTemp, Warning, TEXT("FindSessions ..."));

				Ptr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionsComplete);
				Ptr->FindSessions(0, SearchSettings.ToSharedRef());
			}
		}
	}
}

void UEOSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	//PRINTF(TEXT("OnFindSessionsComplete: Success %d"), bWasSuccessful);
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete: Success %d"), bWasSuccessful);

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete: found %d lobbies"), SearchSettings->SearchResults.Num());

		if (IOnlineSessionPtr Ptr = OnlineSubsystem->GetSessionInterface())
		{
			if (SearchSettings->SearchResults.Num())
			{
				Ptr->OnJoinSessionCompleteDelegates.AddUObject(this,&UEOSGameInstance::OnJoinSessionComplete);

				for (int i = 0; i < SearchSettings->SearchResults.Num(); ++i) 
				{
					FString tmpSt = SearchSettings->SearchResults[i].GetSessionIdStr();
					UE_LOG(LogTemp, Warning, TEXT("SearchResults [%d] : %s"),i,*tmpSt);
				}

				//PRINTF(TEXT("FindSessions ..."));
				UE_LOG(LogTemp, Warning, TEXT("JoinSessions ..."));
				Ptr->JoinSession(0, TestSessionName, SearchSettings->SearchResults[0]);
			}
		}
	}

	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr Ptr = OnlineSubsystem->GetSessionInterface())
		{
			Ptr->ClearOnFindSessionsCompleteDelegates(this);
		}
	}
}

void UEOSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{
	if (JoinResult == EOnJoinSessionCompleteResult::Type::Success)
	{
		//PRINTF(TEXT("OnJoinSessionComplete: Success"));
		UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete: Success"));
	}

	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete: %s "), *SessionName.ToString());

	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr Ptr = OnlineSubsystem->GetSessionInterface())
		{
			FString ConnectionString = FString();
			Ptr->GetResolvedConnectString(SessionName, ConnectionString);
			if (!ConnectionString.IsEmpty())
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionString: %s"), *ConnectionString);

				if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					UE_LOG(LogTemp, Warning, TEXT("ClientTravel: %s"), *ConnectionString);

					FString ipstr;

					FString leftString;
					FString rightString;
					bool is = ConnectionString.Split(":", &leftString, &rightString);
					FString myfinalIP = leftString + FString(":7777");
					UE_LOG(LogTemp, Warning, TEXT("leftString: %s ,rightString: %s "), *leftString,*rightString);
					UE_LOG(LogTemp, Warning, TEXT("myfinalIP: %s  "), *myfinalIP);
					if (isLAN) 
					{
						if (IsIPNot0)
						{
							GetWorld()->GetFirstPlayerController()->ClientTravel(myfinalIP, TRAVEL_Absolute);
						}
						else
						{
							GetWorld()->GetFirstPlayerController()->ClientTravel(ConnectionString, TRAVEL_Absolute);
						}
					}
					else
					{
						GetWorld()->GetFirstPlayerController()->ClientTravel(ConnectionString, TRAVEL_Absolute);
					}
					//PC->ClientTravel(ConnectionString, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
}

void UEOSGameInstance::GetAllFriends()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineFriendsPtr Ptr = OnlineSubsystem->GetFriendsInterface())
			{
				Ptr->ReadFriendsList(0, FString(""), FOnReadFriendsListComplete::CreateUObject(this, &UEOSGameInstance::OnReadFriendsListComplete));
			}
		}
	}
}

void UEOSGameInstance::OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	//PRINTF(TEXT("OnReadFriendsListComplete: Success %d, %s"), bWasSuccessful, *ListName);

	UE_LOG(LogTemp, Warning, TEXT("OnReadFriendsListComplete: Success %d, %s"), bWasSuccessful, *ListName);
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnReadFriendsListComplete: failure error %s"), *ErrorStr);
		//PRINTF(TEXT("OnReadFriendsListComplete: failure error %s"), *ErrorStr);
	}

	if (bWasSuccessful)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineFriendsPtr Ptr = OnlineSubsystem->GetFriendsInterface())
			{
				TArray<TSharedRef<FOnlineFriend>> Friends;
				if (Ptr->GetFriendsList(0, ListName, Friends))
				{
					for (TSharedRef<FOnlineFriend> it : Friends)
					{
						FString FriendName = it.Get().GetDisplayName();
						//PRINTF(TEXT("FriendName %s"), *FriendName);
						UE_LOG(LogTemp, Warning, TEXT("FriendName %s"), *FriendName);
					}
				}
				else
				{
					//PRINTF(TEXT("GetFriendsList failed"));
					UE_LOG(LogTemp, Warning, TEXT("GetFriendsList failed"));
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Was Successful At Getting FriendsList"));
	}
}

void UEOSGameInstance::ShowInviteUI()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineExternalUIPtr Ptr = OnlineSubsystem->GetExternalUIInterface())
			{
				Ptr->ShowInviteUI(0, TestSessionName);
			}
		}
	}
	else
	{
		//PRINTF(TEXT("ShowInviteUI: not logged in"));
		UE_LOG(LogTemp, Warning, TEXT("ShowInviteUI: not logged in"));
	}
}

void UEOSGameInstance::ShowFriendsUI()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineExternalUIPtr Ptr = OnlineSubsystem->GetExternalUIInterface())
			{
				Ptr->ShowFriendsUI(0);
			}
		}
	}
	else
	{
		//PRINTF(TEXT("ShowInviteUI: not logged in"));
		UE_LOG(LogTemp, Warning, TEXT("ShowFriendsUI: not logged in"));
	}
}

void UEOSGameInstance::ShowLoginUI()
{
	if (true)
	{
		if (OnlineSubsystem)
		{
			//FOnlineSubsystemSteamPtr* steamptr = Cast<FOnlineSubsystemSteamPtr*>(OnlineSubsystem);
			//FOnlineAuthSteamPtr AuthSteamPtr IsUsingSteamNetworking();

			if (IOnlineExternalUIPtr Ptr = OnlineSubsystem->GetExternalUIInterface())
			{
				Ptr->ShowLoginUI(0,false,true);
			}
		}
	}
	else
	{
		//PRINTF(TEXT("ShowInviteUI: not logged in"));
		UE_LOG(LogTemp, Warning, TEXT("ShowFriendsUI: not logged in"));
	}
}

void UEOSGameInstance::SetIsIPNot0(bool isstr)
{
	IsIPNot0 = isstr;

	UE_LOG(LogTemp, Warning, TEXT("IsIPNot0 is %d"), IsIPNot0);
}

void UEOSGameInstance::SetMyisLAN(bool isstr)
{
	isLAN = isstr;
	
	UE_LOG(LogTemp, Warning, TEXT("isLAN is %d"), isLAN);
}

void UEOSGameInstance::SetMyisSearchLAN(bool isstr)
{
	isSearchLAN = isstr;
	UE_LOG(LogTemp, Warning, TEXT("isSearchLAN is %d"), isSearchLAN);
}

void UEOSGameInstance::SetMyIsDedicated(bool isstr)
{
	IsDedicated = isstr;
	UE_LOG(LogTemp, Warning, TEXT("IsDedicated is %d"), IsDedicated);
}

void UEOSGameInstance::SetMyTestAccount(FString str)
{
	MyTestAccount = str;
}

void UEOSGameInstance::OnCreateSteamSessionComplete(FName SessionName, bool Succeeded)
{
	if (Succeeded)
	{
		//GetWorld()->ServerTravel("ThirdPersonMap?listen");
		GetWorld()->ServerTravel(FString("ThirdPersonMap?listen"), false);
		
	}
}

void UEOSGameInstance::OnFindSteamSessionComplete(bool Succeeded)
{
	if (Succeeded)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		if (SearchResults.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("JoinSession"));
			SessionInterface->JoinSession(0, FName("Crete Session"), SearchResults[0]);
		}
	}
}

void UEOSGameInstance::OnJoinSteamSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "")
		{
			UE_LOG(LogTemp, Warning, TEXT("JoinSession Start"));
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UEOSGameInstance::CreateSteamServer()
{
	UE_LOG(LogTemp, Warning, TEXT("CreateServer"));
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = IsDedicated;
	//SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSettings.bIsLANMatch = isLAN;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 5;

	SessionInterface->CreateSession(0, FName("Crete Session"), SessionSettings);
}

void UEOSGameInstance::JoinSteamServer()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	//SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->bIsLanQuery = isSearchLAN;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);

	UE_LOG(LogTemp, Warning, TEXT("JoinServer"));

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UEOSGameInstance::ConnectIP(FString ip)
{
	UE_LOG(LogTemp, Warning, TEXT("Start ConnectIP!"));
	GetWorld()->GetFirstPlayerController()->ClientTravel(ip, TRAVEL_Absolute);
}

void UEOSGameInstance::ConnectSessionIP(FString ip)
{
	if (bIsLoggedIn) {
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				UE_LOG(LogTemp, Warning, TEXT("CreateSession Start!"));

				FOnlineSessionSettings SessionSettings;
				SessionSettings.bIsDedicated = true;
				SessionSettings.bShouldAdvertise = true;

				//SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
				SessionSettings.bIsLANMatch = isLAN;

				SessionSettings.NumPublicConnections = 7;
				SessionSettings.bAllowJoinInProgress = true;
				SessionSettings.bAllowJoinViaPresence = true;
				SessionSettings.bAllowInvites = true;
				SessionSettings.bUsesPresence = true;

				SessionSettings.bUseLobbiesIfAvailable = true; // lobby相关
				// 设置成可以portal在Lobby中找到
				SessionSettings.Set(SEARCH_KEYWORDS, FString("MyEOSTestLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

				SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnConnectSessionIPComplete);
				MyIP = ip;
				SessionPtr->CreateSession(0, TestSessionName, SessionSettings);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Create Session: Not Logged In"));
	}
}

void UEOSGameInstance::OnConnectSessionIPComplete(FName SessionName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete: %s Success %d"), *SessionName.ToString(), Succeeded);
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
			// 跳转
			//GetWorld()->ServerTravel(FString("MyWorld?listen"), false);
			//GetWorld()->ServerTravel(FString("ThirdPersonMap?listen"), false);
			UE_LOG(LogTemp, Warning, TEXT("Start to ClientTravel"));
			UE_LOG(LogTemp, Warning, TEXT("MyIP is : %s"),*MyIP);
			GetWorld()->GetFirstPlayerController()->ClientTravel(MyIP, TRAVEL_Absolute);
			UE_LOG(LogTemp, Warning, TEXT("End to ClientTravel"));

		}
	}
}

void UEOSGameInstance::TravelToGameMap()
{
	UE_LOG(LogTemp, Warning, TEXT("All Player Travel Succeed?"));

	GetWorld()->ServerTravel(FString("GameMap"), false);
	//UWorld::ServerTravel();
}