void recursive_function()
{
   lock();
   recursive_function();
   unlock();
}

void recursive_function_entry()
{
   lock();
   recursive_function();
   unlock();
}

void recursive_function()
{
   recursive_function();
}

