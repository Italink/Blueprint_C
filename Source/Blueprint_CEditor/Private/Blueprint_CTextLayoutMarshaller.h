#pragma once
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/SyntaxHighlighterTextLayoutMarshaller.h"

class FTextLayout;

class FWhiteSpaceTextRun : public FSlateTextRun
{
public:
	static TSharedRef< FWhiteSpaceTextRun > Create(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style, const FTextRange& InRange, int32 NumSpacesPerTab);

public:
	virtual FVector2D Measure(int32 StartIndex, int32 EndIndex, float Scale, const FRunTextContext& TextContext) const override;

protected:
	FWhiteSpaceTextRun(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& InStyle, const FTextRange& InRange, int32 InNumSpacesPerTab);

private:
	int32 NumSpacesPerTab;

	float TabWidth;

	float SpaceWidth;
};

class FBlueprint_CTextLayoutMarshaller : public FSyntaxHighlighterTextLayoutMarshaller
{
public:

	struct FSyntaxTextStyle
	{
		FSyntaxTextStyle();

		FSyntaxTextStyle(const FTextBlockStyle& InNormalTextStyle, const FTextBlockStyle& InOperatorTextStyle, const FTextBlockStyle& InKeywordTextStyle, const FTextBlockStyle& InStringTextStyle, const FTextBlockStyle& InNumberTextStyle, const FTextBlockStyle& InCommentTextStyle, const FTextBlockStyle& InPreProcessorKeywordTextStyle);

		FTextBlockStyle NormalTextStyle;
		FTextBlockStyle OperatorTextStyle;
		FTextBlockStyle KeywordTextStyle;
		FTextBlockStyle StringTextStyle;
		FTextBlockStyle NumberTextStyle;
		FTextBlockStyle CommentTextStyle;
		FTextBlockStyle PreProcessorKeywordTextStyle;
		FTextBlockStyle MetaDataTextStyle;
	};

	static TSharedRef<FBlueprint_CTextLayoutMarshaller> Create(const FSyntaxTextStyle& InSyntaxTextStyle);

	virtual ~FBlueprint_CTextLayoutMarshaller();

protected:

	virtual void ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout, TArray<ISyntaxTokenizer::FTokenizedLine> TokenizedLines) override;

	FBlueprint_CTextLayoutMarshaller(TSharedPtr< ISyntaxTokenizer > InTokenizer, const FSyntaxTextStyle& InSyntaxTextStyle);

	/** Styles used to display the text */
	FSyntaxTextStyle SyntaxTextStyle;

	/** String representing tabs */
	FString TabString;
};
