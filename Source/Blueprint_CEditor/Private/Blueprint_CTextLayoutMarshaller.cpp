#include "Blueprint_CTextLayoutMarshaller.h"
#include "Fonts/FontMeasure.h"

#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

TSharedRef< FWhiteSpaceTextRun > FWhiteSpaceTextRun::Create(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style, const FTextRange& InRange, int32 NumSpacesPerTab)
{
	return MakeShareable(new FWhiteSpaceTextRun(InRunInfo, InText, Style, InRange, NumSpacesPerTab));
}

FVector2D FWhiteSpaceTextRun::Measure(int32 StartIndex, int32 EndIndex, float Scale, const FRunTextContext& TextContext) const
{
	const FVector2D ShadowOffsetToApply((EndIndex == Range.EndIndex) ? FMath::Abs(Style.ShadowOffset.X * Scale) : 0.0f, FMath::Abs(Style.ShadowOffset.Y * Scale));

	if (EndIndex - StartIndex == 0 || Text->IsEmpty())
	{
		return FVector2D(ShadowOffsetToApply.X * Scale, GetMaxHeight(Scale));
	}

	// count tabs
	int32 TabCount = 0;
	for (int32 Index = StartIndex; Index < EndIndex; Index++)
	{
		if ((*Text)[Index] == TEXT('\t'))
		{
			TabCount++;
		}
	}

	const TSharedRef< FSlateFontMeasure > FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D Size = FontMeasure->Measure(**Text, StartIndex, EndIndex, Style.Font, true, Scale) + ShadowOffsetToApply;

	Size.X -= TabWidth * (float)TabCount * Scale;
	Size.X += SpaceWidth * (float)(TabCount * NumSpacesPerTab) * Scale;

	return Size;
}

FWhiteSpaceTextRun::FWhiteSpaceTextRun(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& InStyle, const FTextRange& InRange, int32 InNumSpacesPerTab)
	: FSlateTextRun(InRunInfo, InText, InStyle, InRange)
	, NumSpacesPerTab(InNumSpacesPerTab)
{
	// measure tab width
	const TSharedRef< FSlateFontMeasure > FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	TabWidth = FontMeasure->Measure(TEXT("\t"), 0, 1, Style.Font, true, 1.0f).X;
	SpaceWidth = FontMeasure->Measure(TEXT(" "), 0, 1, Style.Font, true, 1.0f).X;
}

const TCHAR* Keywords[] =
{
	TEXT("alignas"),
	TEXT("alignof"),
	TEXT("and"),
	TEXT("and_eq"),
	TEXT("asm"),
	TEXT("auto"),
	TEXT("bitand"),
	TEXT("bitor"),
	TEXT("bool"),
	TEXT("break"),
	TEXT("case"),
	TEXT("catch"),
	TEXT("char"),
	TEXT("char16_t"),
	TEXT("char32_t"),
	TEXT("class"),
	TEXT("compl"),
	TEXT("concept"),
	TEXT("const"),
	TEXT("constexpr"),
	TEXT("const_cast"),
	TEXT("continue"),
	TEXT("decltype"),
	TEXT("default"),
	TEXT("delete"),
	TEXT("double"),
	TEXT("dynamic_cast"),
	TEXT("else"),
	TEXT("enum"),
	TEXT("explicit"),
	TEXT("export"),
	TEXT("extern"),
	TEXT("false"),
	TEXT("float"),
	TEXT("for"),
	TEXT("friend"),
	TEXT("goto"),
	TEXT("if"),
	TEXT("inline"),
	TEXT("int64"),
	TEXT("int32"),
	TEXT("int16"),
	TEXT("int8"),
	TEXT("int "),
	TEXT("uint64"),
	TEXT("uint32"),
	TEXT("uint16"),
	TEXT("uint8"),
	TEXT("uint "),
	TEXT("long"),
	TEXT("mutable"),
	TEXT("namespace"),
	TEXT("new"),
	TEXT("noexcept"),
	TEXT("not"),
	TEXT("not_eq"),
	TEXT("nullptr"),
	TEXT("operator"),
	TEXT("or"),
	TEXT("or_eq"),
	TEXT("private"),
	TEXT("protected"),
	TEXT("public"),
	TEXT("register"),
	TEXT("reinterpret_cast"),
	TEXT("requires"),
	TEXT("return"),
	TEXT("short"),
	TEXT("signed"),
	TEXT("sizeof"),
	TEXT("static"),
	TEXT("static_assert"),
	TEXT("static_cast"),
	TEXT("struct"),
	TEXT("switch"),
	TEXT("template"),
	TEXT("this"),
	TEXT("thread_local"),
	TEXT("throw"),
	TEXT("true"),
	TEXT("try"),
	TEXT("typedef"),
	TEXT("typeid"),
	TEXT("typename"),
	TEXT("union"),
	TEXT("unsigned"),
	TEXT("using"),
	TEXT("virtual"),
	TEXT("void"),
	TEXT("volatile"),
	TEXT("wchar_t"),
	TEXT("while"),
	TEXT("xor"),
	TEXT("xor_eq"),
};

const TCHAR* Operators[] =
{
	TEXT("/*"),
	TEXT("*/"),
	TEXT("//"),
	TEXT("\""),
	TEXT("\'"),
	TEXT("::"),
	TEXT(":"),
	TEXT("+="),
	TEXT("++"),
	TEXT("+"),
	TEXT("--"),
	TEXT("-="),
	TEXT("-"),
	TEXT("("),
	TEXT(")"),
	TEXT("["),
	TEXT("]"),
	TEXT("."),
	TEXT("->"),
	TEXT("!="),
	TEXT("!"),
	TEXT("&="),
	TEXT("~"),
	TEXT("&"),
	TEXT("*="),
	TEXT("*"),
	TEXT("->"),
	TEXT("/="),
	TEXT("/"),
	TEXT("%="),
	TEXT("%"),
	TEXT("<<="),
	TEXT("<<"),
	TEXT("<="),
	TEXT("<"),
	TEXT(">>="),
	TEXT(">>"),
	TEXT(">="),
	TEXT(">"),
	TEXT("=="),
	TEXT("&&"),
	TEXT("&"),
	TEXT("^="),
	TEXT("^"),
	TEXT("|="),
	TEXT("||"),
	TEXT("|"),
	TEXT("?"),
	TEXT("="),
	TEXT(","),
	TEXT("{"),
	TEXT("}"),
	TEXT(";"),
};

const TCHAR* PreProcessorKeywords[] =
{
	TEXT("#include"),
	TEXT("#define"),
	TEXT("#ifndef"),
	TEXT("#ifdef"),
	TEXT("#if"),
	TEXT("#else"),
	TEXT("#endif"),
	TEXT("#pragma"),
	TEXT("#undef"),

	TEXT("UPROPERTY"),
	TEXT("UFUNCTION"),
};

FBlueprint_CTextLayoutMarshaller::FSyntaxTextStyle::FSyntaxTextStyle()
{
	const FSlateFontInfo Consolas10 = DEFAULT_FONT("Mono", 9);
	const FTextBlockStyle NormalText = FTextBlockStyle()
		.SetFont(Consolas10)
		.SetColorAndOpacity(FLinearColor::White)
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::Black)
		.SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f));
	NormalTextStyle = FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(FColor(0xffefefef)));
	OperatorTextStyle = FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(FColor(0xffcfcfcf)));
	KeywordTextStyle = FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(FColor(0xff006ab4)));
	StringTextStyle = FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(FColor(0xffeba30a)));
	NumberTextStyle = FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(FColor(0xff6db3a8)));
	CommentTextStyle = FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(FColor(0xff57a64a)));
	PreProcessorKeywordTextStyle = FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(FColor(0xffa678dc)));
	MetaDataTextStyle = FTextBlockStyle(NormalText).SetColorAndOpacity(FLinearColor(FColor(0xff9f9f9f)));
}

FBlueprint_CTextLayoutMarshaller::FSyntaxTextStyle::FSyntaxTextStyle(const FTextBlockStyle& InNormalTextStyle, const FTextBlockStyle& InOperatorTextStyle, const FTextBlockStyle& InKeywordTextStyle, const FTextBlockStyle& InStringTextStyle, const FTextBlockStyle& InNumberTextStyle, const FTextBlockStyle& InCommentTextStyle, const FTextBlockStyle& InPreProcessorKeywordTextStyle) : NormalTextStyle(InNormalTextStyle)
, OperatorTextStyle(InOperatorTextStyle)
, KeywordTextStyle(InKeywordTextStyle)
, StringTextStyle(InStringTextStyle)
, NumberTextStyle(InNumberTextStyle)
, CommentTextStyle(InCommentTextStyle)
, PreProcessorKeywordTextStyle(InPreProcessorKeywordTextStyle)
{

}


TSharedRef< FBlueprint_CTextLayoutMarshaller > FBlueprint_CTextLayoutMarshaller::Create(const FSyntaxTextStyle& InSyntaxTextStyle)
{
	TArray<FSyntaxTokenizer::FRule> TokenizerRules;

	// operators
	for(const auto& Operator : Operators)
	{
		TokenizerRules.Emplace(FSyntaxTokenizer::FRule(Operator));
	}	

	// keywords
	for(const auto& Keyword : Keywords)
	{
		TokenizerRules.Emplace(FSyntaxTokenizer::FRule(Keyword));
	}

	// Pre-processor Keywords
	for(const auto& PreProcessorKeyword : PreProcessorKeywords)
	{
		TokenizerRules.Emplace(FSyntaxTokenizer::FRule(PreProcessorKeyword));
	}

	return MakeShareable(new FBlueprint_CTextLayoutMarshaller(FSyntaxTokenizer::Create(TokenizerRules), InSyntaxTextStyle));
}

FBlueprint_CTextLayoutMarshaller::~FBlueprint_CTextLayoutMarshaller()
{

}

void FBlueprint_CTextLayoutMarshaller::ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout, TArray<ISyntaxTokenizer::FTokenizedLine> TokenizedLines)
{
	enum class EParseState : uint8
	{
		None,
		LookingForString,
		LookingForCharacter,
		LookingForBracket,
		LookingForSingleLineComment,
		LookingForMultiLineComment,
	};

	TArray<FTextLayout::FNewLineData> LinesToAdd;
	LinesToAdd.Reserve(TokenizedLines.Num());

	// Parse the tokens, generating the styled runs for each line
	EParseState ParseState = EParseState::None;
	for(const ISyntaxTokenizer::FTokenizedLine& TokenizedLine : TokenizedLines)
	{
		TSharedRef<FString> ModelString = MakeShareable(new FString());
		TArray<TSharedRef<IRun>> Runs;
		int BracketCount = 0;

		if(ParseState == EParseState::LookingForSingleLineComment)
		{
			ParseState = EParseState::None;
		}

		for(const ISyntaxTokenizer::FToken& Token : TokenizedLine.Tokens)
		{
			const FString TokenText = SourceString.Mid(Token.Range.BeginIndex, Token.Range.Len());
			const FTextRange ModelRange(ModelString->Len(), ModelString->Len() + TokenText.Len());
			ModelString->Append(TokenText);

			FRunInfo RunInfo(TEXT("SyntaxHighlight.CPP.Normal"));
			FTextBlockStyle TextBlockStyle = SyntaxTextStyle.NormalTextStyle;

			bool bIsWhitespace = FString(TokenText).TrimEnd().IsEmpty();
			if(!bIsWhitespace)
			{
				bool bHasMatchedSyntax = false;
				if(Token.Type == ISyntaxTokenizer::ETokenType::Syntax)
				{
					if(ParseState == EParseState::None && TokenText == TEXT("\""))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::LookingForString;
						bHasMatchedSyntax = true;
					}
					else if(ParseState == EParseState::LookingForString && TokenText == TEXT("\""))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Normal");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::None;
					}
					else if(ParseState == EParseState::None && TokenText == TEXT("\'"))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::LookingForCharacter;
						bHasMatchedSyntax = true;
					}
					else if(ParseState == EParseState::LookingForCharacter && TokenText == TEXT("\'"))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Normal");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::None;
					}
					else if(ParseState == EParseState::None && TokenText.StartsWith(TEXT("#")))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.PreProcessorKeyword");
						TextBlockStyle = SyntaxTextStyle.PreProcessorKeywordTextStyle;
						ParseState = EParseState::None;
					}
					else if (ParseState == EParseState::None && (TokenText.StartsWith(TEXT("UPROPERTY")) || TokenText.StartsWith(TEXT("UFUNCTION"))))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.PreProcessorKeyword");
						TextBlockStyle = SyntaxTextStyle.PreProcessorKeywordTextStyle;
						ParseState = EParseState::LookingForBracket;
						BracketCount = 0;
						bHasMatchedSyntax = true;
					}
					else if (ParseState == EParseState::LookingForBracket)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::LookingForBracket;
						if (TokenText.StartsWith(TEXT("("))) 
						{
							BracketCount++;
						}
						else if (TokenText.StartsWith(TEXT(")"))) 
						{
							BracketCount--;
							if (BracketCount == 0)
							{
								ParseState = EParseState::None;
								BracketCount = 0;
							}
						}
					}
					else if(ParseState == EParseState::None && TokenText == TEXT("//"))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Comment");
						TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
						ParseState = EParseState::LookingForSingleLineComment;
					}
					else if(ParseState == EParseState::None && TokenText == TEXT("/*"))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Comment");
						TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
						ParseState = EParseState::LookingForMultiLineComment;
					}
					else if(ParseState == EParseState::LookingForMultiLineComment && TokenText == TEXT("*/"))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Comment");
						TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
						ParseState = EParseState::None;
					}
					else if(ParseState == EParseState::None && TChar<WIDECHAR>::IsAlpha(TokenText[0]))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Keyword");
						TextBlockStyle = SyntaxTextStyle.KeywordTextStyle;
						ParseState = EParseState::None;
					}
					else if(ParseState == EParseState::None && !TChar<WIDECHAR>::IsAlpha(TokenText[0]))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Operator");
						TextBlockStyle = SyntaxTextStyle.OperatorTextStyle;
						ParseState = EParseState::None;
					}
				}
				
				// It's possible that we fail to match a syntax token if we're in a state where it isn't parsed
				// In this case, we treat it as a literal token
				if(Token.Type == ISyntaxTokenizer::ETokenType::Literal || !bHasMatchedSyntax)
				{
					if(ParseState == EParseState::LookingForString)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
					}
					else if(ParseState == EParseState::LookingForCharacter)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
					}
					else if(ParseState == EParseState::LookingForSingleLineComment)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Comment");
						TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
					}
					else if(ParseState == EParseState::LookingForMultiLineComment)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Comment");
						TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
					}
					else if (ParseState == EParseState::LookingForBracket)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.CPP.Comment");
						TextBlockStyle = SyntaxTextStyle.MetaDataTextStyle;
					}
				}

				TSharedRef<ISlateRun > Run = FSlateTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange);
				Runs.Add(Run);
			}
			else
			{
				RunInfo.Name = TEXT("SyntaxHighlight.CPP.WhiteSpace");
				TSharedRef< ISlateRun > Run = FWhiteSpaceTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange, 4);
				Runs.Add(Run);
			}
		}

		LinesToAdd.Emplace(MoveTemp(ModelString), MoveTemp(Runs));
	}

	TargetTextLayout.AddLines(LinesToAdd);
}

FBlueprint_CTextLayoutMarshaller::FBlueprint_CTextLayoutMarshaller(TSharedPtr< ISyntaxTokenizer > InTokenizer, const FSyntaxTextStyle& InSyntaxTextStyle)
	: FSyntaxHighlighterTextLayoutMarshaller(MoveTemp(InTokenizer))
	, SyntaxTextStyle(InSyntaxTextStyle)
{
}
