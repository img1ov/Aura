// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Interaction/CombatInterface.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* NewTask = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return NewTask;
}

void UTargetDataUnderMouse::Activate()
{
	// 在Client中的GameplayTask激活时，需要给服务器发送本地信息，比如鼠标指针位置
	// Client执行Task Active后会将执行数据通过RPC发给服务器复现
	// GAS封装了一套专门用于将客户端的数据传输给服务器的方法，里面绑定了各种发送和接收的触发回调
	// 也就是通过AbilityTargetData来对要发送的信息进行封装，再执行AbilitySystemComponent->ServerSetReplicatedTargetData来将数据发送给服务器，注意，该方法会由额外一个RPC来发送
	// 客户端执行的Task Active为一个RPC， 传输数据的ServerSetReplicatedTargetData又是另一个RPC，服务器接受这两个RPC的时序可能不同
	// 当服务器先收到TargetData时，ASC会将该数据先缓存起来，此时数据时有效状态，等Active RPC收到时再去使用
	// 但如果服务器先执行客户端发来的Active，此时还没收到TargetData的RPC，此时数据无效，无法处理该数据
	// 为了防止这种RPC时序的不稳定性，GAS提供了一套委托机制，如果服务器先收到Client Active的执行RPC，则可以提前绑定该接收委托并等待，在数据到达时在进行广播

	// 注意:
	// 在UE5.4之前需要手动初始化TargetData缓存，否则缓存不会分配空间导致数据失效，可以在引擎初始化时手动初始化缓存，例如在AssetManager中手动调用UAbilitySystemGlobals::InitGlobalData()
	// 当前版本为5.6，GAS在初始化时会自动执行该方法，所以无需再次手动调用一道
	
	//如果为Client，给服务器发送指针位置数据
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SendMouseCorsorData();
	}
	//为服务器时接收数据
	else
	{
		//绑定接收委托
		FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		FPredictionKey PredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		//如果数数据提前到达，无需继续等待，立即触发绑定的委托
		if (AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey))
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCorsorData()
{
	//将接下来ASC执行的所有打包发送方法视为同一个预测键
	FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get());
	
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	//TargetData包装，GAS提供了一个专门用来发送HitResult数据的包装器
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	
	Data->HitResult = CursorHit;
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(Data);
	FGameplayTag ApplicationTag;

	//将包装后的数据发给服务器
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		TargetDataHandle,
		ApplicationTag,
		AbilitySystemComponent->ScopedPredictionKey
		);

	//如果技能没有突然中断或被取消，则在本地优先执行一道该Task
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(TargetDataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	//如果数据先到达服务器，ASC会将该数据缓存起来，此时由于数据已经到达了，这时可以将缓存清掉
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
