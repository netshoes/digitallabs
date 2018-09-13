using System;
using System.Collections.Generic;
using BotTemplate.Core.Dialogs;
using BotTemplate.Core.IntentMapper;
using BotTemplate.Core.NaturalLanguage;
using BotTemplate.Dialogs;
using Microsoft.Bot.Builder.Dialogs;

namespace bottemplate.intentmapping
{
    [Serializable]
    public class GenericIntentMappers : IntentMapper
    {
        public override Dictionary<string, InitialIntent> SupportedIntenties => new Dictionary<string, InitialIntent> {
            { "DuvidaPisada", InitDuvidaPisadaDialog },
            //{ "InatividadePOC", InitInatividadeDialog },
            { "Palmeiras", InitPalmeirasDialog },
            { "Greetings", InitGreetingsDialog },
            { "None",  InitNoneDialog }
        };

        private IDialog InitGreetingsDialog(NaturalLanguageResult result)
        {
            return new GreetingsDialog();
        }

        private IDialog InitNoneDialog(NaturalLanguageResult result)
        {
            return new NoneDialog();
        }

        private IDialog InitDuvidaPisadaDialog(NaturalLanguageResult result)
        {
            return new DuvidaPisadaDialog();
        }

        private static IDialog InitInatividadeDialog(NaturalLanguageResult result)
        {
            return new InatividadeDialog();
        }

        private static IDialog InitPalmeirasDialog(NaturalLanguageResult result)
        {
            return new PalmeirasDialog();
        }
    }
}