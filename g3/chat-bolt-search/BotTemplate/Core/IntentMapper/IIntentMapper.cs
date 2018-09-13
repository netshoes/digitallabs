using BotTemplate.Core.Dialogs;
using System.Collections.Generic;
using BotTemplate.Core.NaturalLanguage;

namespace BotTemplate.Core.IntentMapper
{
    public interface IIntentMapper
    {
        Dictionary<string, InitialIntent> SupportedIntenties { get; }
        InitialIntent FindCanHandleIntent(NaturalLanguageIntent Intent);
    }
}
