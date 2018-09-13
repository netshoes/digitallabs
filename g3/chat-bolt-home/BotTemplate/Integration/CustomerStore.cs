using BotTemplate.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace BotTemplate.Integration
{
    [Serializable]
    public class CustomerStore
    {
        private List<Customer> _customers;
        public CustomerStore()
        {
            _customers = new List<Customer>();

            _customers.Add(new Customer("07.526.557/0001-00", "AMBEV"));
            _customers.Add(new Customer("61.189.288/0001-89", "LOJAS MARISA"));
            _customers.Add(new Customer("16.590.234/0001-76", "AREZZO INDÚSTRIA E COMÉRCIO"));
            _customers.Add(new Customer("09.305.994/0001-29", "AZUL"));
            _customers.Add(new Customer("67.620.377/0001-14", "MINERVA"));
            _customers.Add(new Customer("42.150.391/0001-70", "BRASKEM"));
            _customers.Add(new Customer("00.776.574/0001-56", "B2W – COMPANHIA DIGITAL"));
            _customers.Add(new Customer("61.695.227/0001-93", "ELETROPAULO"));
            _customers.Add(new Customer("78.876.950/0001-71", "ELETROBRAS"));
            _customers.Add(new Customer("07.689.002/0001-89", "HERING"));
            _customers.Add(new Customer("02.916.265/0001-60", "JBS"));
            _customers.Add(new Customer("02.916.265/0001-62", "JBS2"));
            _customers.Add(new Customer("52.548.435/0001-79", "JSL"));
            _customers.Add(new Customer("33.014.556/0001-96", "LOJAS AMERICANAS"));
            _customers.Add(new Customer("92.754.738/0001-62", "LOJAS RENNER"));
            _customers.Add(new Customer("47.960.950/0001-21", "MAGAZINE LUIZA"));
            _customers.Add(new Customer("08.343.492/0001-20", "MRV ENGENHARIA"));
            _customers.Add(new Customer("58.119.199/0001-51", "ODONTOPREV"));
            _customers.Add(new Customer("33.000.167/0001-01", "PETROLEO BRASILEIRO PETROBRAS"));
            _customers.Add(new Customer("51.466.860/0001-56", "SAO MARTINHO"));
            _customers.Add(new Customer("29.978.814/0001-87", "SUL AMERICA"));
            _customers.Add(new Customer("02.558.115/0001-21", "TIM"));
            _customers.Add(new Customer("60.894.730/0001-05", "USINAS SIDERÚRGICAS DE MINAS GERAIS – USIMINAS"));
            _customers.Add(new Customer("33.592.510/0001-54", "VALE"));            
            _customers.Add(new Customer("02.558.157/0001-62", "TELEFÔNICA BRASIL"));            
        }

        public List<Customer> GetCustomers(string name)
        {
            List<Customer> customers = new List<Customer>();
            if(name.Equals(""))
            {
                return customers;
            }
            foreach (Customer c in _customers)
            {
                if (c.name.Contains(name.ToUpper()))
                {
                    customers.Add(c);
                }                
            }
            return customers;
        }

    }
}