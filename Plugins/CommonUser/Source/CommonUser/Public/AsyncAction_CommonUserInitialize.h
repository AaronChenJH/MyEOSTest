// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "CommonUserSubsystem.h"

#include "AsyncAction_CommonUserInitialize.generated.h"

/**
 * Async action to handle different functions for initializing users
 */
UCLASS()
class COMMONUSER_API UAsyncAction_CommonUserInitialize : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	/**
	 * Initializes a local player with the common user system, which includes doing platform-specific login and privilege checks.
	 * 使用通用用户系统初始化本地播放器，包括执行平台特定的登录和特权检查。
	 * When the process has succeeded or failed, it will broadcast the OnInitializationComplete delegate.
	 * 当进程成功或失败时，它将广播OnInitializationComplete委托。	
	 *
	 * @param LocalPlayerIndex	Desired index of ULocalPlayer in Game Instance, 0 will be primary player and 1+ for local multiplayer
	 * @param ControllerId		Number of local user according to online system, will match physical Controller Id
	 * @param bCanUseGuestLogin	If true, this player can be a guest without a real system net id
	 */
	UFUNCTION(BlueprintCallable, Category = CommonUser, meta=(BlueprintInternalUseOnly="true"))
	static UAsyncAction_CommonUserInitialize* InitializeForLocalPlay(UCommonUserSubsystem* Target, int32 LocalPlayerIndex, int32 ControllerId, bool bCanUseGuestLogin);

	/**
	 * Attempts to log an existing user into the platform-specific online backend to enable full online play
	 * 尝试将现有用户登录到平台特定的在线后端，以启用完全在线播放
	 * When the process has succeeded or failed, it will broadcast the OnInitializationComplete delegate.
	 * 当进程成功或失败时，它将广播OnInitializationComplete委托。
	 *
	 * @param LocalPlayerIndex	Index of existing LocalPlayer in Game Instance
	 */
	UFUNCTION(BlueprintCallable, Category = CommonUser, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncAction_CommonUserInitialize* LoginForOnlinePlay(UCommonUserSubsystem* Target, int32 LocalPlayerIndex);

	/** Call when initialization succeeds or fails */
	UPROPERTY(BlueprintAssignable)
	FCommonUserOnInitializeCompleteMulticast OnInitializationComplete;

	/** Fail and send callbacks if needed */
	void HandleFailure();

	/** Wrapper delegate, will pass on to OnInitializationComplete if appropriate */
	UFUNCTION()
	virtual void HandleInitializationComplete(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext);

protected:
	/** Actually start the initialization */
	virtual void Activate() override;

	TWeakObjectPtr<UCommonUserSubsystem> Subsystem;
	FCommonUserInitializeParams Params;
};
