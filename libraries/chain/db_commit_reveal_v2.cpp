/**
 * The Revolution Populi Project
 * Copyright (C) 2020 Revolution Populi Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <graphene/chain/database.hpp>
#include <graphene/chain/hardfork.hpp>

namespace graphene { namespace chain {

fc::optional<commit_reveal_v2_object> database::get_account_commit_reveal_v2( const account_id_type account ) const
{
   const auto& cr_idx = get_index_type<commit_reveal_v2_index>();
   const auto& by_op_idx = cr_idx.indices().get<by_account>();
   auto itr = by_op_idx.lower_bound(account);

   if( itr != by_op_idx.end() && itr->account == account )
   {
      return fc::optional<commit_reveal_v2_object>(*itr);
   }
   return fc::optional<commit_reveal_v2_object>();
}

vector<commit_reveal_v2_object> database::get_commit_reveals_v2( const commit_reveal_v2_id_type start, uint32_t limit ) const
{
   const auto& cr_idx = get_index_type<commit_reveal_v2_index>();
   const auto& by_op_idx = cr_idx.indices().get<by_id>();
   auto itr = by_op_idx.lower_bound(start);

   vector<commit_reveal_v2_object> result;
   while( itr != by_op_idx.end() && limit-- )
   {
      result.push_back(*itr);
      ++itr;
   }
   return result;
}

uint64_t database::get_commit_reveal_seed_v2(const vector<account_id_type>& accounts) const
{
   const auto& cr_idx = get_index_type<commit_reveal_v2_index>();
   const auto& by_op_idx = cr_idx.indices().get<by_account>();

   uint64_t seed = 0;
   uint32_t maintenance_time = get_dynamic_global_properties().next_maintenance_time.sec_since_epoch();
   if (HARDFORK_REVPOP_13_PASSED(head_block_time()))
   {
   uint32_t prev_maintenance_time = maintenance_time - get_global_properties().parameters.maintenance_interval;
   for (const auto& acc: accounts){
      auto itr = by_op_idx.lower_bound(acc);
      if( itr != by_op_idx.end() && itr->account == acc
         && prev_maintenance_time <= itr->maintenance_time
         && itr->maintenance_time <  maintenance_time )
      {
         seed += itr->value;
      }
   }
   }
   else
   {
   for (const auto& acc: accounts){
      auto itr = by_op_idx.lower_bound(acc);
      if( itr != by_op_idx.end() && itr->account == acc
         && itr->maintenance_time == maintenance_time)
      {
         seed += itr->value;
      }
   }
   }
   return seed;
}

vector<account_id_type> database::filter_commit_reveal_participant_v2(const vector<account_id_type>& accounts) const
{
   const auto& cr_idx = get_index_type<commit_reveal_v2_index>();
   const auto& by_op_idx = cr_idx.indices().get<by_account>();

   vector<account_id_type> result;
   uint32_t maintenance_time = get_dynamic_global_properties().next_maintenance_time.sec_since_epoch();
   if (HARDFORK_REVPOP_13_PASSED(head_block_time()))
   {
   uint32_t prev_maintenance_time = maintenance_time - get_global_properties().parameters.maintenance_interval;
   for (const auto& acc: accounts){
      auto itr = by_op_idx.lower_bound(acc);
      if( itr != by_op_idx.end() && itr->account == acc && itr->value != 0
         && prev_maintenance_time <= itr->maintenance_time
         && itr->maintenance_time <  maintenance_time )

      {
         result.push_back(itr->account);
      }
   }
   }
   else
   {
   for (const auto& acc: accounts){
      auto itr = by_op_idx.lower_bound(acc);
      if( itr != by_op_idx.end() && itr->account == acc && itr->value != 0
         && itr->maintenance_time == maintenance_time)

      {
         result.push_back(itr->account);
      }
   }
   }
   return result;
}

}}
