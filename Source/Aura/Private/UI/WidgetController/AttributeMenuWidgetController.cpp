// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeInfo)
	
	if (UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	{
		for (TTuple<FGameplayTag, FGameplayAttribute(*)()>& Pair : AuraAttributeSet->TagsToAttributeMap)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
				[this, Pair](const FOnAttributeChangeData& Data)
				{
					BroadCastAttributeInfo(Pair.Key, Data.NewValue);
				}
			);
		}
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues() const
{
	check(AttributeInfo)
	
	if (UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	{
		//遍历TagsToAttributeMap映射表
		for (TTuple<FGameplayTag, FGameplayAttribute(*)()>& Pair : AuraAttributeSet->TagsToAttributeMap)
		{
			//为每一个GameplayTag映射到的UI Widget同步数据
			BroadCastAttributeInfo(Pair.Key, Pair.Value().GetNumericValue(AuraAttributeSet));
		}
	}
}

void UAttributeMenuWidgetController::BroadCastAttributeInfo(const FGameplayTag& GameplayTag, const float& NewNumericValue) const
{
	//获取本地GameplayTag-UI配置数据
	FAuraAttributeInfo AuraAttributeInfo = AttributeInfo->FindAttributeInfoForTag(GameplayTag);
	//获取到配置数据后更改数值
	AuraAttributeInfo.AttributeNumericValue = NewNumericValue;
	//最终广播到UI层
	AttributeInfoDelegate.Broadcast(AuraAttributeInfo);
}
