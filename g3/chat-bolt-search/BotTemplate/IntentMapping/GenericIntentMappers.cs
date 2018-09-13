using System;
using BotTemplate.Dialogs;
using BotTemplate.Core.Dialogs;
using System.Collections.Generic;
using Microsoft.Bot.Builder.Dialogs;
using BotTemplate.Core.IntentMapper;
using BotTemplate.Core.NaturalLanguage;
using BotTemplate.Integration;
using Autofac;

namespace bottemplate.intentmapping
{
    [Serializable]
    public class GenericIntentMappers : IntentMapper
    {
        public override Dictionary<string, InitialIntent> SupportedIntenties => new Dictionary<string, InitialIntent> {
            { "Greetings", InitGreetingsDialog },
            { "DuvidaPisada", DuvidaPisada},
            { "ProcuraProduto", ProcuraProdutoDialog},
            { "None",  InitNoneDialog }
        };

        private IDialog InitGreetingsDialog(NaturalLanguageResult result)
        {
            return new GreetingsDialog(result);
        }

        private IDialog InitNoneDialog(NaturalLanguageResult result)
        {
            return new NoneDialog();
        }

        private IDialog DuvidaPisada(NaturalLanguageResult result)
        {
            return new DuvidaPisadaDialog(result);
        }
        private IDialog ProcuraProdutoDialog(NaturalLanguageResult result)
        {
            return new ProcuraProdutoDialog(result);
        }


    }
}