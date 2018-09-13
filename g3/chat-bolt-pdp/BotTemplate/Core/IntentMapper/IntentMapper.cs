using System;
using BotTemplate.Core.Dialogs;
using System.Collections.Generic;
using BotTemplate.Core.NaturalLanguage;

namespace BotTemplate.Core.IntentMapper
{
    [Serializable]
    public abstract class IntentMapper : IIntentMapper
    {
        public abstract Dictionary<string, InitialIntent> SupportedIntenties { get; }

        public InitialIntent FindCanHandleIntent(NaturalLanguageIntent Intent)
        {
            InitialIntent initDialog;
            if (SupportedIntenties.TryGetValue(Intent.Intent, out initDialog))
            {
                return initDialog;
            }
            return SupportedIntenties["None"];
        }

    }
}